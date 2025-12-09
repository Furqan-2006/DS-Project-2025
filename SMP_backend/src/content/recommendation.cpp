#include "content/recommendation.hpp"
#include "ADT/hash_map.hpp"
#include "ADT/set.hpp"
#include "ADT/queue.hpp"
#include <iostream>

RecommendationEngine::RecommendationEngine(PostManager *pm, RelationshipGraph *rg)
    : pm(pm), rg(rg) {}

// ============================================================================
// User Recommendations using BFS depth-2 traversal (Friends of Friends)
// ============================================================================
std::vector<ull> RecommendationEngine::recommendUsers(ull userID, size_t limit) const
{
    // Use BFS depth-2 traversal to find friends of friends
    HashMap<ull, int> candidateFrequency; // Count how many paths lead to each candidate
    Set<ull> visited;                     // Avoid cycles
    Queue<std::pair<ull, int>> q;         // (nodeID, depth)

    // Start BFS from the user
    q.enqueue({userID, 0});
    visited.insert(userID);

    // Get direct friends (depth 1) - we don't want to recommend them
    const Set<NodeID> *directFriends = rg->getFollowing(userID);
    Set<ull> alreadyFollowing;
    if (directFriends)
    {
        const auto &data = directFriends->data();
        for (size_t i = 0; i < directFriends->size(); i++)
        {
            alreadyFollowing.insert(data[i]);
        }
    }

    // BFS traversal with depth limit of 2
    while (!q.isEmpty())
    {
        auto current = q.front();
        q.dequeue();

        ull node = current.first;
        int depth = current.second;

        // Only traverse up to depth 2
        if (depth >= 2)
            continue;

        // Get neighbors of current node
        const Set<NodeID> *neighbors = rg->getFollowing(node);
        if (!neighbors)
            continue;

        const auto &data = neighbors->data();
        for (size_t i = 0; i < neighbors->size(); i++)
        {
            ull neighbor = data[i];

            // Count frequency for depth-2 nodes (friends of friends)
            if (depth == 1)
            {
                // This is a friend of friend
                if (neighbor != userID && !alreadyFollowing.contains(neighbor))
                {
                    // Increment frequency count
                    int *freq = candidateFrequency.get(neighbor);
                    if (freq)
                        candidateFrequency.insert(neighbor, *freq + 1);
                    else
                        candidateFrequency.insert(neighbor, 1);
                }
            }

            // Continue BFS traversal (avoid cycles using visited set)
            if (!visited.contains(neighbor))
            {
                visited.insert(neighbor);
                q.enqueue({neighbor, depth + 1});
            }
        }
    }

    // Top-K ranking: Sort candidates by frequency (intersection count)
    std::vector<std::pair<ull, int>> candidates;
    for (auto it = candidateFrequency.begin(); it != candidateFrequency.end(); ++it)
    {
        candidates.push_back({(*it).key, (*it).value});
    }

    // Manual quicksort by frequency (descending)
    if (candidates.size() > 1)
    {
        auto sortFunc = [](std::vector<std::pair<ull, int>> &arr, int low, int high, auto &self) -> void
        {
            if (low < high)
            {
                auto pivot = arr[high];
                int i = low - 1;

                for (int j = low; j < high; j++)
                {
                    // Sort by frequency descending
                    if (arr[j].second > pivot.second)
                    {
                        i++;
                        auto temp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = temp;
                    }
                }

                auto temp = arr[i + 1];
                arr[i + 1] = arr[high];
                arr[high] = temp;

                int pi = i + 1;
                self(arr, low, pi - 1, self);
                self(arr, pi + 1, high, self);
            }
        };

        sortFunc(candidates, 0, candidates.size() - 1, sortFunc);
    }

    // Return top-K results
    std::vector<ull> result;
    for (size_t i = 0; i < candidates.size() && i < limit; i++)
    {
        result.push_back(candidates[i].first);
    }

    return result;
}

// ============================================================================
// Post Recommendations based on friend network
// ============================================================================
std::vector<Post *> RecommendationEngine::recommendPosts(ull userID, size_t limit) const
{
    HashMap<ull, bool> seen; // Avoid duplicate posts
    std::vector<Post *> result;

    // Get posts from friends of friends using BFS
    Set<ull> visited;
    Queue<std::pair<ull, int>> q;

    q.enqueue({userID, 0});
    visited.insert(userID);

    // Get direct friends
    const Set<NodeID> *directFriends = rg->getFollowing(userID);
    Set<ull> friends;
    if (directFriends)
    {
        const auto &data = directFriends->data();
        for (size_t i = 0; i < directFriends->size(); i++)
        {
            friends.insert(data[i]);
        }
    }

    // BFS depth-2 to collect candidate users
    std::vector<ull> candidateUsers;

    while (!q.isEmpty())
    {
        auto current = q.front();
        q.dequeue();

        ull node = current.first;
        int depth = current.second;

        if (depth >= 2)
            continue;

        const Set<NodeID> *neighbors = rg->getFollowing(node);
        if (!neighbors)
            continue;

        const auto &data = neighbors->data();
        for (size_t i = 0; i < neighbors->size(); i++)
        {
            ull neighbor = data[i];

            // Collect posts from depth-1 and depth-2 users
            if (depth >= 0 && neighbor != userID)
            {
                candidateUsers.push_back(neighbor);
            }

            if (!visited.contains(neighbor))
            {
                visited.insert(neighbor);
                q.enqueue({neighbor, depth + 1});
            }
        }
    }

    // Get posts from candidate users
    for (size_t i = 0; i < candidateUsers.size(); i++)
    {
        std::vector<Post *> posts = pm->getPostsByUser(candidateUsers[i]);

        for (size_t j = 0; j < posts.size(); j++)
        {
            ull postID = posts[j]->getPostID();

            if (!seen.contains(postID))
            {
                result.push_back(posts[j]);
                seen.insert(postID, true);

                if (result.size() >= limit)
                    return result;
            }
        }
    }

    return result;
}

// ============================================================================
// Trending Posts using Top-K ranking by likes
// ============================================================================
std::vector<Post *> RecommendationEngine::recommendTrendingPosts(size_t limit) const
{
    std::vector<Post *> all = pm->getAllPosts();

    // Top-K selection using manual sorting by likes count
    if (all.size() > 1)
    {
        auto sortFunc = [](std::vector<Post *> &arr, int low, int high, auto &self) -> void
        {
            if (low < high)
            {
                Post *pivot = arr[high];
                int i = low - 1;

                for (int j = low; j < high; j++)
                {
                    // Sort by likes descending
                    if (arr[j]->getLikesCount() > pivot->getLikesCount())
                    {
                        i++;
                        Post *temp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = temp;
                    }
                }

                Post *temp = arr[i + 1];
                arr[i + 1] = arr[high];
                arr[high] = temp;

                int pi = i + 1;
                self(arr, low, pi - 1, self);
                self(arr, pi + 1, high, self);
            }
        };

        sortFunc(all, 0, all.size() - 1, sortFunc);
    }

    // Return top-K results
    std::vector<Post *> result;
    for (size_t i = 0; i < all.size() && i < limit; i++)
    {
        result.push_back(all[i]);
    }

    return result;
}

// ============================================================================
// Display Functions
// ============================================================================
void RecommendationEngine::displayRecommendedPosts(ull userID, size_t limit) const
{
    std::vector<Post *> posts = recommendPosts(userID, limit);

    std::cout << "=== Recommended Posts for User " << userID << " ===\n";
    for (size_t i = 0; i < posts.size(); i++)
    {
        std::cout << (i + 1) << ". ";
        posts[i]->display();
    }
}

void RecommendationEngine::displayRecommendedUsers(ull userID, size_t limit) const
{
    std::vector<ull> users = recommendUsers(userID, limit);

    std::cout << "=== Recommended Users for User " << userID << " ===\n";
    for (size_t i = 0; i < users.size(); i++)
    {
        std::cout << (i + 1) << ". User ID: " << users[i] << "\n";
    }
}