#include "core/relationGraph.hpp"

RelationshipGraph::RelationshipGraph(size_t reserve = 1000) : likesGraph(reserve), followsGraph(reserve), activeGraph(reserve) {}

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