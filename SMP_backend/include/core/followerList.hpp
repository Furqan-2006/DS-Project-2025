#pragma once

#include "ADT/hash_map.hpp"
#include "ADT/linked_list.hpp"

typedef unsigned long long ull;

class FollowerList
{
private:
    HashMap<ull, LinkedList<ull> *> followersMap;

public:
    FollowerList();
    ~FollowerList();

    void addFollower(ull userID, ull followerID);
    void removeFollower(ull userID, ull followerID);

    LinkedList<ull> *getFollowers(ull userID);
    const LinkedList<ull> *getFollowers(ull userID) const;
    size_t followerCount(ull userID) const;

    bool hasFollower(ull userID, ull followerID) const;

    ull *findFollowerByUsername(ull userID, const char *username,
                                const HashMap<ull, const char *> &usernames);

    void removeUser(ull userID);
};