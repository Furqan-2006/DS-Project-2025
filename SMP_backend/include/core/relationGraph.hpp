#pragma once

#include "ADT/graph.hpp"
#include "ADT/queue.hpp"
#include "ADT/hash_map.hpp"
#include <vector>
#include <functional>

enum class RelationType
{
    FOLLOWS,
    LIKES,
    ACTIVE
};

struct ActiveEdge
{
    NodeID u1;
    NodeID u2;
    long long timestamp;
};

struct RecommendationScore
{
    NodeID user;
    double score;
    std::string reason;
};

class RelationshipGraph
{
private:
    Graph likesGraph;
    Graph activeGraph;
    Graph followsGraph;
    Queue<ActiveEdge> activeWindow;

    // Helper methods for traversal and recommendations
    void bfsHelper(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth = -1) const;
    void dfsHelper(NodeID start, HashMap<NodeID, bool> &visited,
                   std::function<bool(NodeID, int)> visitor, int depth = 0, int maxDepth = -1) const;

public:
    RelationshipGraph(size_t reserve);

    // User and Post Registration
    void registerUser(NodeID user);
    void registerPost(NodeID post);

    // Connection Management
    bool follow(NodeID follower, NodeID followee);
    bool unfollow(NodeID follower, NodeID followee);
    bool isFollowing(NodeID follower, NodeID followee) const;

    // Followers/Following
    const Set<NodeID> *getFollowers(NodeID user) const;
    const Set<NodeID> *getFollowing(NodeID user) const;
    size_t followerCount(NodeID user) const;
    size_t followingCount(NodeID user) const;

    // Mutual Connections & Friends
    std::vector<NodeID> getFriends(NodeID user) const;
    std::vector<NodeID> getMutualConnections(NodeID user1, NodeID user2) const;

    // Post Likes
    bool likePost(NodeID user, NodeID post);
    bool unlikePost(NodeID user, NodeID post);
    bool hasLiked(NodeID user, NodeID post) const;
    const Set<NodeID> *getPostLikes(NodeID post) const;
    size_t likedPostsCount(NodeID user) const;

    // Active Users (Time-based)
    void addActive(NodeID u1, NodeID u2, long long now);
    void expireActive(long long now);
    const Set<NodeID> *getActiveWith(NodeID user) const;
    void clearActive();

    // Network Traversal (BFS/DFS)
    void bfs(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth = -1) const;
    void dfs(NodeID start, std::function<bool(NodeID, int)> visitor, int maxDepth = -1) const;
    std::vector<NodeID> getReachableUsers(NodeID start, int maxDepth = -1) const;

    // Friend-of-Friend
    std::vector<NodeID> getFriendOfFriend(NodeID user, int maxDepth = 2) const;
    HashMap<NodeID, int> getFoFWithDistance(NodeID user, int maxDepth = 2) const;

    // Graph-based Recommendation Heuristics
    std::vector<RecommendationScore> recommendUsers(NodeID user, size_t limit = 10) const;
    std::vector<RecommendationScore> recommendByMutualFriends(NodeID user, size_t limit = 10) const;
    std::vector<RecommendationScore> recommendByCommonInterests(NodeID user, size_t limit = 10) const;
    std::vector<RecommendationScore> recommendByPopularity(NodeID user, size_t limit = 10) const;

    // Cycle Detection
    bool hasCycle() const;
    std::vector<std::vector<NodeID>> findAllCycles() const;

    // Network Statistics
    double getClusteringCoefficient(NodeID user) const;
    int getShortestPathLength(NodeID from, NodeID to) const;
};