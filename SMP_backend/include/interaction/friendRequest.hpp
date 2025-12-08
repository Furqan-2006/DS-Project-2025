#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class RequestStatus
{
    PENDING,
    ACCEPTED,
    REJECTED,
    CANCELLED
};
class FriendRequest
{
private:
    unsigned long long r_id;
    unsigned long long s_id;
    RequestStatus s;
    std::time_t timeStamp;

    static std::string statusToString(RequestStatus st);
    static RequestStatus stringToType(const std::string &st);

public:
    FriendRequest();
    FriendRequest(unsigned long long sid, unsigned long long rid);

    unsigned long long getRecieverID() const;
    unsigned long long getSenderID() const;
    RequestStatus getStatus() const;
    std::time_t getTimeStamp() const;

    void accept();
    void reject();
    void cancel();

    json toJSON() const;
    static FriendRequest fromJSON(const json &j);
};

class FriendRequestManager
{
private:
        std::vector<FriendRequest> outbox;
    std::vector<FriendRequest> inbox;

    std::string filePath;

public:
    FriendRequestManager(const std::string &filePath);

    bool sendRequest(unsigned long long sid, unsigned long long rid);

    bool acceptRequest(unsigned long long sid);
    bool rejectRequest(unsigned long long sid);
    bool cancelRequest(unsigned long long rid);

    std::vector<FriendRequest> getInbox() const;
    std::vector<FriendRequest> getOutbox() const;

    bool hasPenIn(unsigned long long sid) const;
    bool hasPenOut(unsigned long long rid) const;

    bool saveToFile() const;
    bool loadFromFile();
};