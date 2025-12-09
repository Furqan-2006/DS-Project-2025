#include "core/status.hpp"

StatusManager::StatusManager() {}

void StatusManager::setOnline(ull userID)
{
    statusMap[userID] = true;
}

void StatusManager::setOffline(ull userID)
{
    statusMap[userID] = false;
}

bool StatusManager::isOnline(ull userID) const
{
    const bool *val = statusMap.get(userID);
    return val ? *val : false;
}

const char* StatusManager::getStatusString(ull userID) const
{
    return isOnline(userID) ? "ONLINE" : "OFFLINE";
}

void StatusManager::removeUser(ull userID)
{
    statusMap.remove(userID);
}