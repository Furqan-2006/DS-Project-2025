#pragma once

#include "relationGraph.hpp"

class FollowerList
{
private:
    const RelationshipGraph &rg;

public:
    explicit FollowerList(const RelationshipGraph &graph);

    const Set<NodeID> *getFollowers(NodeID user) const;
    const Set<NodeID> *getFollowing(NodeID user) const;

    size_t followerCount(NodeID user) const;
    size_t followingCount(NodeID user) const;
};