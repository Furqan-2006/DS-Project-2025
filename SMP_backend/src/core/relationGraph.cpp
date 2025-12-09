#include "core/relationGraph.hpp"
#include "ADT/hash_map.hpp"
#include "ADT/set.hpp"
#include "ADT/queue.hpp"
#include <cmath>

// Manual sorting helper (QuickSort)
template <typename T, typename Compare>
void quickSort(std::vector<T> &arr, int low, int high, Compare comp)
{
    if (low < high)
    {
        T pivot = arr[high];
        int i = low - 1;

        for (int j = low; j < high; j++)
        {
            if (comp(arr[j], pivot))
            {
                i++;
                T temp = arr[i];
                arr[i] = arr[j];
                arr[j] = temp;
            }
        }

        T temp = arr[i + 1];
        arr[i + 1] = arr[high];
        arr[high] = temp;

        int pi = i + 1;
        quickSort(arr, low, pi - 1, comp);
        quickSort(arr, pi + 1, high, comp);
    }
}

template <typename T, typename Compare>
void customSort(std::vector<T> &arr, Compare comp)
{
    if (arr.size() > 1)
    {
        quickSort(arr, 0, arr.size() - 1, comp);
    }
}

RelationshipGraph::RelationshipGraph(size_t reserve)
    : likesGraph(reserve), followsGraph(reserve), activeGraph(reserve) {}

// ============================================================================
// User and Post Registration
// ============================================================================

void RelationshipGraph::registerUser(NodeID user)
{
    followsGraph.addNode(user);
    likesGraph.addNode(user);
    activeGraph.addNode(user);
}

void RelationshipGraph::registerPost(NodeID post)
{
    likesGraph.addNode(post);
}

// ============================================================================
// Connection Management
// ============================================================================

bool RelationshipGraph::follow(NodeID follower, NodeID followee)
{
    return followsGraph.addEdge(follower, followee);
}

bool RelationshipGraph::unfollow(NodeID follower, NodeID followee)
{
    return followsGraph.removeEdge(follower, followee);
}

bool RelationshipGraph::isFollowing(NodeID follower, NodeID followee) const
{
    return followsGraph.hasEdge(follower, followee);
}

const Set<NodeID> *RelationshipGraph::getFollowers(NodeID user) const
{
    return followsGraph.inNeighbors(user);
}

const Set<NodeID> *RelationshipGraph::getFollowing(NodeID user) const
{
    return followsGraph.outNeighbors(user);
}

size_t RelationshipGraph::followerCount(NodeID user) const
{
    return followsGraph.inDegree(user);
}

size_t RelationshipGraph::followingCount(NodeID user) const
{
    return followsGraph.outDegree(user);
}

// ============================================================================
// Mutual Connections & Friends
// ============================================================================

std::vector<NodeID> RelationshipGraph::getFriends(NodeID user) const
{
    std::vector<NodeID> friends;

    const Set<NodeID> *following = followsGraph.outNeighbors(user);
    if (!following)
        return friends;

    auto &data = following->data();

    for (size_t i = 0; i < following->size(); i++)
    {
        NodeID other = data[i];
        if (followsGraph.hasEdge(other, user))
        {
            friends.push_back(other);
        }
    }
    return friends;
}

std::vector<NodeID> RelationshipGraph::getMutualConnections(NodeID user1, NodeID user2) const
{
    std::vector<NodeID> mutuals;

    const Set<NodeID> *following1 = followsGraph.outNeighbors(user1);
    const Set<NodeID> *following2 = followsGraph.outNeighbors(user2);

    if (!following1 || !following2)
        return mutuals;

    const auto &data = following1->data();

    for (size_t i = 0; i < following1->size(); i++)
    {
        NodeID id = data[i];
        if (following2->contains(id))
            mutuals.push_back(id);
    }
    return mutuals;
}

// ============================================================================
// Post Likes
// ============================================================================

bool RelationshipGraph::likePost(NodeID user, NodeID post)
{
    return likesGraph.addEdge(user, post);
}

bool RelationshipGraph::unlikePost(NodeID user, NodeID post)
{
    return likesGraph.removeEdge(user, post);
}

bool RelationshipGraph::hasLiked(NodeID user, NodeID post) const
{
    return likesGraph.hasEdge(user, post);
}

const Set<NodeID> *RelationshipGraph::getPostLikes(NodeID post) const
{
    return likesGraph.inNeighbors(post);
}

size_t RelationshipGraph::likedPostsCount(NodeID user) const
{
    return likesGraph.outDegree(user);
}

// ============================================================================
// Active Users (Time-based)
// ============================================================================

void RelationshipGraph::addActive(NodeID u1, NodeID u2, long long now)
{
    activeGraph.addNode(u1);
    activeGraph.addNode(u2);

    activeGraph.addEdge(u1, u2);
    activeGraph.addEdge(u2, u1);

    activeWindow.enqueue({u1, u2, now});
}

void RelationshipGraph::expireActive(long long now)
{
    while (!activeWindow.isEmpty())
    {
        const ActiveEdge &e = activeWindow.front();
        if (now - e.timestamp <= 500)
            break;
        activeGraph.removeEdge(e.u1, e.u2);
        activeGraph.removeEdge(e.u2, e.u1);
        activeWindow.dequeue();
    }
}

const Set<NodeID> *RelationshipGraph::getActiveWith(NodeID user) const
{
    return activeGraph.outNeighbors(user);
}

void RelationshipGraph::clearActive()
{
    activeGraph.clear();

    while (!activeWindow.isEmpty())
    {
        activeWindow.dequeue();
    }
}

// ============================================================================
// Network Traversal - BFS
// ============================================================================

void RelationshipGraph::bfsHelper(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth) const
{
    Queue<std::pair<NodeID, int>> q;
    Set<NodeID> visited;

    q.enqueue({start, 0});
    visited.insert(start);

    while (!q.isEmpty())
    {
        auto current = q.front();
        q.dequeue();

        NodeID node = current.first;
        int depth = current.second;

        // Call visitor; if it returns false, stop traversal
        if (!visitor(node, depth))
            return;

        // Check depth limit
        if (maxDepth != -1 && depth >= maxDepth)
            continue;

        // Visit neighbors
        const Set<NodeID> *neighbors = followsGraph.outNeighbors(node);
        if (!neighbors)
            continue;

        const auto &data = neighbors->data();
        for (size_t i = 0; i < neighbors->size(); i++)
        {
            NodeID neighbor = data[i];
            if (!visited.contains(neighbor))
            {
                visited.insert(neighbor);
                q.enqueue({neighbor, depth + 1});
            }
        }
    }
}

void RelationshipGraph::bfs(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth) const
{
    bfsHelper(start, visitor, maxDepth);
}

std::vector<NodeID> RelationshipGraph::getReachableUsers(NodeID start, int maxDepth) const
{
    std::vector<NodeID> reachable;
    bfsHelper(start, [&](NodeID node, int depth)
              {
        if (node != start)
            reachable.push_back(node);
        return true; }, maxDepth);
    return reachable;
}

// ============================================================================
// Network Traversal - DFS
// ============================================================================

void RelationshipGraph::dfsHelper(NodeID current, HashMap<NodeID, bool> &visited,
                                  std::function<bool(NodeID, int)> visitor, int depth, int maxDepth) const
{
    visited.insert(current, true);

    // Call visitor; if it returns false, stop traversal
    if (!visitor(current, depth))
        return;

    // Check depth limit
    if (maxDepth != -1 && depth >= maxDepth)
        return;

    // Visit neighbors
    const Set<NodeID> *neighbors = followsGraph.outNeighbors(current);
    if (!neighbors)
        return;

    const auto &data = neighbors->data();
    for (size_t i = 0; i < neighbors->size(); i++)
    {
        NodeID neighbor = data[i];
        bool *isVisited = visited.get(neighbor);
        if (!isVisited || !(*isVisited))
        {
            dfsHelper(neighbor, visited, visitor, depth + 1, maxDepth);
        }
    }
}

void RelationshipGraph::dfs(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth) const
{
    HashMap<NodeID, bool> visited;
    dfsHelper(start, visited, visitor, 0, maxDepth);
}

// ============================================================================
// Friend-of-Friend
// ============================================================================

std::vector<NodeID> RelationshipGraph::getFriendOfFriend(NodeID user, int maxDepth) const
{
    Set<NodeID> fofSet;
    Set<NodeID> directFriends;

    // Get direct friends (depth 1)
    const Set<NodeID> *following = followsGraph.outNeighbors(user);
    if (following)
    {
        const auto &data = following->data();
        for (size_t i = 0; i < following->size(); i++)
        {
            directFriends.insert(data[i]);
        }
    }

    // BFS to find friends-of-friends
    bfsHelper(user, [&](NodeID node, int depth)
              {
        if (depth > 0 && depth <= maxDepth && node != user)
        {
            // Exclude direct friends and self
            if (!directFriends.contains(node))
            {
                fofSet.insert(node);
            }
        }
        return true; }, maxDepth);

    // Convert Set to vector
    std::vector<NodeID> result;
    const auto &fofData = fofSet.data();
    for (size_t i = 0; i < fofSet.size(); i++)
    {
        result.push_back(fofData[i]);
    }
    return result;
}

HashMap<NodeID, int> RelationshipGraph::getFoFWithDistance(NodeID user, int maxDepth) const
{
    HashMap<NodeID, int> distances;

    bfsHelper(user, [&](NodeID node, int depth)
              {
        if (node != user && depth > 0 && depth <= maxDepth)
        {
            distances.insert(node, depth);
        }
        return true; }, maxDepth);

    return distances;
}

// ============================================================================
// Graph-based Recommendation Heuristics
// ============================================================================

std::vector<RecommendationScore> RelationshipGraph::recommendUsers(NodeID user, size_t limit) const
{
    // Combined recommendation strategy
    HashMap<NodeID, double> scores;
    HashMap<NodeID, std::string> reasons;

    // 1. Friend-of-Friend (weight: 0.4)
    auto fofDist = getFoFWithDistance(user, 2);
    for (auto it = fofDist.begin(); it != fofDist.end(); ++it)
    {
        NodeID fof = (*it).key;
        int dist = (*it).value;

        if (!isFollowing(user, fof))
        {
            scores.insert(fof, 0.4 / dist);
            reasons.insert(fof, "Friend of friend");
        }
    }

    // 2. Mutual friends (weight: 0.35)
    const Set<NodeID> *following = followsGraph.outNeighbors(user);
    if (following)
    {
        const auto &data = following->data();
        for (size_t i = 0; i < following->size(); i++)
        {
            NodeID friend_id = data[i];
            const Set<NodeID> *friendFollowing = followsGraph.outNeighbors(friend_id);
            if (friendFollowing)
            {
                const auto &friendData = friendFollowing->data();
                for (size_t j = 0; j < friendFollowing->size(); j++)
                {
                    NodeID candidate = friendData[j];
                    if (candidate != user && !isFollowing(user, candidate))
                    {
                        double *currentScore = scores.get(candidate);
                        if (currentScore)
                            scores[candidate] = *currentScore + 0.35;
                        else
                            scores.insert(candidate, 0.35);

                        if (!reasons.contains(candidate))
                            reasons.insert(candidate, "Mutual connections");
                    }
                }
            }
        }
    }

    // 3. Popularity (weight: 0.25)
    for (auto it = scores.begin(); it != scores.end(); ++it)
    {
        NodeID candidate = (*it).key;
        size_t followers = followerCount(candidate);
        scores[candidate] = (*it).value + 0.25 * std::log(followers + 1) / 10.0;
    }

    // Convert to vector and sort
    std::vector<RecommendationScore> recommendations;
    for (auto it = scores.begin(); it != scores.end(); ++it)
    {
        NodeID uid = (*it).key;
        double score = (*it).value;
        std::string *reason = reasons.get(uid);
        recommendations.push_back({uid, score, reason ? *reason : ""});
    }

    customSort(recommendations, [](const RecommendationScore &a, const RecommendationScore &b)
               { return a.score > b.score; });

    if (recommendations.size() > limit)
        recommendations.resize(limit);

    return recommendations;
}

std::vector<RecommendationScore> RelationshipGraph::recommendByMutualFriends(NodeID user, size_t limit) const
{
    HashMap<NodeID, int> mutualCount;

    const Set<NodeID> *following = followsGraph.outNeighbors(user);
    if (!following)
        return {};

    const auto &data = following->data();
    for (size_t i = 0; i < following->size(); i++)
    {
        NodeID friend_id = data[i];
        const Set<NodeID> *friendFollowing = followsGraph.outNeighbors(friend_id);
        if (!friendFollowing)
            continue;

        const auto &friendData = friendFollowing->data();
        for (size_t j = 0; j < friendFollowing->size(); j++)
        {
            NodeID candidate = friendData[j];
            if (candidate != user && !isFollowing(user, candidate))
            {
                int *count = mutualCount.get(candidate);
                if (count)
                    mutualCount.insert(candidate, *count + 1);
                else
                    mutualCount.insert(candidate, 1);
            }
        }
    }

    std::vector<RecommendationScore> recommendations;
    for (auto it = mutualCount.begin(); it != mutualCount.end(); ++it)
    {
        NodeID uid = (*it).key;
        int count = (*it).value;
        recommendations.push_back({uid, static_cast<double>(count),
                                   std::to_string(count) + " mutual friend(s)"});
    }

    customSort(recommendations, [](const RecommendationScore &a, const RecommendationScore &b)
               { return a.score > b.score; });

    if (recommendations.size() > limit)
        recommendations.resize(limit);

    return recommendations;
}

std::vector<RecommendationScore> RelationshipGraph::recommendByCommonInterests(NodeID user, size_t limit) const
{
    HashMap<NodeID, int> commonInterests;

    // Get posts liked by user
    const Set<NodeID> *userLikes = likesGraph.outNeighbors(user);
    if (!userLikes)
        return {};

    const auto &likedPosts = userLikes->data();
    for (size_t i = 0; i < userLikes->size(); i++)
    {
        NodeID post = likedPosts[i];
        const Set<NodeID> *postLikers = likesGraph.inNeighbors(post);
        if (!postLikers)
            continue;

        const auto &likers = postLikers->data();
        for (size_t j = 0; j < postLikers->size(); j++)
        {
            NodeID candidate = likers[j];
            if (candidate != user && !isFollowing(user, candidate))
            {
                int *count = commonInterests.get(candidate);
                if (count)
                    commonInterests.insert(candidate, *count + 1);
                else
                    commonInterests.insert(candidate, 1);
            }
        }
    }

    std::vector<RecommendationScore> recommendations;
    for (auto it = commonInterests.begin(); it != commonInterests.end(); ++it)
    {
        NodeID uid = (*it).key;
        int count = (*it).value;
        recommendations.push_back({uid, static_cast<double>(count),
                                   std::to_string(count) + " common interest(s)"});
    }

    customSort(recommendations, [](const RecommendationScore &a, const RecommendationScore &b)
               { return a.score > b.score; });

    if (recommendations.size() > limit)
        recommendations.resize(limit);

    return recommendations;
}

std::vector<RecommendationScore> RelationshipGraph::recommendByPopularity(NodeID user, size_t limit) const
{
    std::vector<RecommendationScore> recommendations;

    // Get all users via FoF or traversal
    auto fofUsers = getFriendOfFriend(user, 3);

    for (size_t i = 0; i < fofUsers.size(); i++)
    {
        NodeID candidate = fofUsers[i];
        if (!isFollowing(user, candidate))
        {
            size_t followers = followerCount(candidate);
            recommendations.push_back({candidate, static_cast<double>(followers),
                                       std::to_string(followers) + " follower(s)"});
        }
    }

    customSort(recommendations, [](const RecommendationScore &a, const RecommendationScore &b)
               { return a.score > b.score; });

    if (recommendations.size() > limit)
        recommendations.resize(limit);

    return recommendations;
}

// ============================================================================
// Cycle Detection
// ============================================================================

bool RelationshipGraph::hasCycle() const
{
    HashMap<NodeID, int> state; // 0 = unvisited, 1 = visiting, 2 = visited

    std::function<bool(NodeID)> dfsCheckCycle = [&](NodeID node) -> bool
    {
        state.insert(node, 1); // Mark as visiting

        const Set<NodeID> *neighbors = followsGraph.outNeighbors(node);
        if (neighbors)
        {
            const auto &data = neighbors->data();
            for (size_t i = 0; i < neighbors->size(); i++)
            {
                NodeID neighbor = data[i];

                auto it = state.find(neighbor);
                if (it != state.end())
                {
                    int neighborState = (*it).value;
                    if (neighborState == 1)
                        return true; // Back edge found - cycle detected
                }
                else if (dfsCheckCycle(neighbor))
                {
                    return true;
                }
            }
        }

        state[node] = 2; // Mark as visited
        return false;
    };

    // Check all nodes (for disconnected components)
    // Note: This is a simplified version. In practice, you'd iterate through all nodes
    // This would require access to all node IDs from the graph

    return false; // Placeholder - requires node enumeration
}

std::vector<std::vector<NodeID>> RelationshipGraph::findAllCycles() const
{
    // Placeholder for cycle finding algorithm (e.g., Johnson's algorithm)
    // This is complex and would require significant implementation
    return {};
}

// ============================================================================
// Network Statistics
// ============================================================================

double RelationshipGraph::getClusteringCoefficient(NodeID user) const
{
    const Set<NodeID> *neighbors = followsGraph.outNeighbors(user);
    if (!neighbors || neighbors->size() < 2)
        return 0.0;

    const auto &data = neighbors->data();
    int connections = 0;
    int possibleConnections = 0;

    for (size_t i = 0; i < neighbors->size(); i++)
    {
        for (size_t j = i + 1; j < neighbors->size(); j++)
        {
            possibleConnections++;
            if (followsGraph.hasEdge(data[i], data[j]) ||
                followsGraph.hasEdge(data[j], data[i]))
            {
                connections++;
            }
        }
    }

    return possibleConnections > 0 ? static_cast<double>(connections) / possibleConnections : 0.0;
}

int RelationshipGraph::getShortestPathLength(NodeID from, NodeID to) const
{
    if (from == to)
        return 0;

    Queue<std::pair<NodeID, int>> q;
    Set<NodeID> visited;

    q.enqueue({from, 0});
    visited.insert(from);

    while (!q.isEmpty())
    {
        auto current = q.front();
        q.dequeue();

        NodeID node = current.first;
        int dist = current.second;

        const Set<NodeID> *neighbors = followsGraph.outNeighbors(node);
        if (!neighbors)
            continue;

        const auto &data = neighbors->data();
        for (size_t i = 0; i < neighbors->size(); i++)
        {
            NodeID neighbor = data[i];
            if (neighbor == to)
                return dist + 1;

            if (!visited.contains(neighbor))
            {
                visited.insert(neighbor);
                q.enqueue({neighbor, dist + 1});
            }
        }
    }

    return -1; // No path found
}