#pragma once

#include <string>
#include <ctime>
#include "nlohmann/json.hpp"

using json = nlohmann::json;

enum class NotificationType
{
    Like,
    Comment,
    Follow,
    Message,
    Mention,
    System
};

class Notification
{
private:
    unsigned long long n_id;
    unsigned long long r_id;
    unsigned long long s_id;
    NotificationType type;
    std::string desc;
    std::time_t timeStamp;
    bool isRead;

    static unsigned long long nextNID;

    static std::string typetostring(NotificationType t);
    static NotificationType stringtotype(const std::string &s);

public:
    Notification();
    Notification(unsigned long long rid, unsigned long long sid, NotificationType t, const std::string &desc);

    unsigned long long getID() const;
    unsigned long long getReciever() const;
    unsigned long long getSender() const;
    NotificationType getType() const;
    std::string getDescription() const;
    std::time_t getTimeStamp() const;
    bool getReadStatus() const;

    void markAsRead();

    json toJSON() const;
    static Notification fromJSON(const json &data);
};

class NotificationManager
{
private:
    Notification **list;
    unsigned long long cap, count;
    struct UnreadNode
    {
        Notification *data;
        UnreadNode *next;
    };
    UnreadNode *head;

    std::string filePath;

    void expand();
    void addToUnread(Notification *n);
    void removeFromUnread(Notification *n);

    NotificationManager(const NotificationManager &) = delete;
    NotificationManager &operator=(const NotificationManager) = delete;

public:
    NotificationManager(const std::string &filePath);
    NotificationManager(const std::string &filePath, unsigned long long initalCap);
    ~NotificationManager();

    void add(Notification *n);
    Notification *create(unsigned long long recieverID, unsigned long long senderID, NotificationType type, const std::string &description);

    Notification *getByID(unsigned long long nid) const;
    Notification **getAll() const;
    Notification **getUnread() const;
    Notification **getByType(NotificationType t) const;

    void markAllRead();
    void markAsRead(unsigned long long nid);
    bool removeByID(unsigned long long nid);

    unsigned long long countUnread() const;

    bool saveToFile() const;
    bool loadFromFile();

    bool empty() const;
    unsigned long long size() const;
};