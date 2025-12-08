#pragma once

#include "ADT/graph.hpp"
#include "ADT/queue.hpp"

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

class RelationshipGraph
{
private:
    Graph likesGraph;
    Graph activeGraph;
    Graph followsGraph;
    Queue<ActiveEdge> activeWindow;

public:
    RelationshipGraph(size_t reserve);

    void registerUser(NodeID user);
    void registerPost(NodeID post);

    bool follow(NodeID follower, NodeID followee);
    bool unfollow(NodeID follower, NodeID followee);
    bool isFollowing(NodeID follower, NodeID followee) const;
    const Set<NodeID> *getFollowers(NodeID user) const;
    const Set<NodeID> *getFollowing(NodeID user) const;
    size_t followerCount(NodeID user) const;
    size_t followingCount(NodeID user) const;

    bool likePost(NodeID user, NodeID post);
    bool unlikePost(NodeID user, NodeID post);
    bool hasLiked(NodeID user, NodeID post) const;
    const Set<NodeID> *getPostLikes(NodeID post) const;
    size_t likeCount(NodeID user) const;

    void addActive(NodeID u1, NodeID u2, long long now);
    void expireActive(long long now);
    const Set<NodeID> *getActiveWith(NodeID user) const;
    void clearActive();
};