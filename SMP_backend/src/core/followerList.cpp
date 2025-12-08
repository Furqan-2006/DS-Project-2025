#include "core/followerList.hpp"

FollowerList::FollowerList(const RelationshipGraph &graph) : rg(graph) {}

const Set<NodeID> *FollowerList::getFollowers(NodeID user) const
{
    return rg.getFollowers(user);
}
const Set<NodeID> *FollowerList::getFollowing(NodeID user) const
{
    return rg.getFollowing(user);
}

size_t FollowerList::followerCount(NodeID user) const
{
    return rg.followerCount(user);
}

size_t FollowerList::followingCount(NodeID user) const
{
    return rg.followingCount(user);
}