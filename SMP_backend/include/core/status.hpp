#pragma once
#include "user.hpp"
#include "ADT/hash_map.hpp"

class StatusManager
{
private:
    HashMap<ull, bool> statusMap;

public:
    StatusManager();

    void setOnline(ull userID);
    void setOffline(ull userID);
    bool isOnline(ull userID) const;
    const char *getStatusString(ull userID) const;
    void removeUser(ull userID);
};