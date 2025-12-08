#include "interaction/friendRequest.hpp"

FriendRequest::FriendRequest() : r_id(0), s_id(0), s(RequestStatus::PENDING), timeStamp((std::time(nullptr))) {}
FriendRequest::FriendRequest(unsigned long long sid, unsigned long long rid)
    : r_id(rid), s_id(sid), s(RequestStatus::PENDING), timeStamp((std::time(nullptr))) {}

unsigned long long FriendRequest::getSenderID() const
{
    return s_id;
}

unsigned long long FriendRequest::getRecieverID() const
{
    return r_id;
}

RequestStatus FriendRequest::getStatus() const
{
    return s;
}

std::time_t FriendRequest::getTimeStamp() const
{
    return timeStamp;
}

void FriendRequest::accept()
{
    s = RequestStatus::ACCEPTED;
}
void FriendRequest::reject()
{
    s = RequestStatus::REJECTED;
}
void FriendRequest::cancel()
{
    s = RequestStatus::CANCELLED;
}

json FriendRequest::toJSON() const
{
    return json{
        {"sender", s_id},
        {"reciever", r_id},
        {"status", statusToString(s)},
        {"timestamp", timeStamp}};
}

FriendRequest FriendRequest::fromJSON(const json &j)
{
    FriendRequest fr;
    fr.s_id = j.at("sender").get<unsigned long long>();
    fr.r_id = j.at("reciever").get<unsigned long long>();
    fr.s = stringToType(j.at("status").get<std::string>());
    fr.timeStamp = j.at("timestamp").get<std::time_t>();

    return fr;
}

// Friend Request Manager implementation

FriendRequestManager::FriendRequestManager(const std::string &fp) : filePath(fp) { loadFromFile(); }

bool FriendRequestManager::sendRequest(unsigned long long sid, unsigned long long rid)
{
    if (sid == rid)
    {
        return false;
    }

    if (hasPenOut(rid) || hasPenIn(sid))
    {
        return false;
    }
    outbox.emplace_back(sid, rid);
    inbox.emplace_back(sid, rid);

    return true;
}

bool FriendRequestManager::acceptRequest(unsigned long long sid)
{
    for (auto &req : inbox)
    {
        if (req.getSenderID() == sid && req.getStatus() == RequestStatus::PENDING)
        {
            req.accept();
            for (auto &out : outbox)
            {
                if (out.getSenderID() == sid && out.getRecieverID() == req.getRecieverID() && out.getStatus() == RequestStatus::PENDING)
                {
                    out.accept();
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

bool FriendRequestManager::rejectRequest(unsigned long long sid)
{
    for (auto &req : inbox)
    {
        if (req.getSenderID() == sid && req.getStatus() == RequestStatus::PENDING)
        {
            req.reject();

            for (auto &out : outbox)
            {
                if (out.getSenderID() == sid && out.getRecieverID() == req.getRecieverID() && out.getStatus() == RequestStatus::PENDING)
                {
                    out.reject();
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

bool FriendRequestManager::cancelRequest(unsigned long long rid)
{
    for (auto &req : outbox)
    {
        if (req.getRecieverID() == rid && req.getStatus() == RequestStatus::PENDING)
        {
            req.cancel();

            for (auto &in : inbox)
            {
                if (in.getRecieverID() == rid && in.getSenderID() == req.getSenderID() && in.getStatus() == RequestStatus::PENDING)
                {
                    in.cancel();
                    break;
                }
            }
            return true;
        }
    }
    return false;
}

std::vector<FriendRequest> FriendRequestManager::getInbox() const
{
    return inbox;
}
std::vector<FriendRequest> FriendRequestManager::getOutbox() const
{
    return outbox;
}

bool FriendRequestManager::hasPenIn(unsigned long long sid) const
{
    for (const auto &req : inbox)
    {
        if (req.getSenderID() == sid && req.getStatus() == RequestStatus::PENDING)
        {
            return true;
        }
    }
    return false;
}

bool FriendRequestManager::hasPenOut(unsigned long long rid) const
{
    for (const auto &req : outbox)
    {
        if (req.getRecieverID() == rid && req.getStatus() == RequestStatus::PENDING)
        {
            return true;
        }
    }
    return false;
}

bool FriendRequestManager::saveToFile() const
{
    json j;
    json inboxArray = json::array();
    json outboxArray = json::array();

    for (const auto &req : inbox)
    {
        inboxArray.push_back(req.toJSON());
    }

    for (const auto &req : outbox)
    {
        outboxArray.push_back(req.toJSON());
    }

    j["inbox"] = inboxArray;
    j["outbox"] = outboxArray;

    std::ofstream file(filePath);
    if (!file.is_open())
        return false;

    file << j.dump(4);
    file.close();
    return true;
}

bool FriendRequestManager::loadFromFile()
{
    std::ifstream file(filePath);
    if (!file.is_open())
        return false;

    json j;
    file >> j;
    file.close();

    inbox.clear();
    outbox.clear();

    if (j.contains("inbox"))
    {
        for (const auto &reqJson : j["inbox"])
        {
            inbox.push_back(FriendRequest::fromJSON(reqJson));
        }
    }

    if (j.contains("outbox"))
    {
        for (const auto &reqJson : j["outbox"])
        {
            outbox.push_back(FriendRequest::fromJSON(reqJson));
        }
    }

    return true;
}