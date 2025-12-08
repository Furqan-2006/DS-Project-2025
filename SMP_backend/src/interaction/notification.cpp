#include "interaction/notification.hpp"
#include <cstring>
#include <fstream>
#include <exception>
#include <iostream>

unsigned long long Notification::nextNID = 1000;

Notification::Notification()
    : n_id(nextNID++), r_id(0), s_id(0), type(NotificationType::System), desc(""), timeStamp(std::time(nullptr)), isRead(false) {}

Notification::Notification(unsigned long long rid, unsigned long long sid, NotificationType t, const std::string &d)
    : n_id(nextNID++), r_id(rid), s_id(sid), type(t), desc(d), timeStamp(std::time(nullptr)), isRead(false) {}

std::string Notification::typetostring(NotificationType t)
{
    switch (t)
    {
    case NotificationType::Like:
        return "like";
    case NotificationType::Comment:
        return "comment";
    case NotificationType::Follow:
        return "follow";
    case NotificationType::Message:
        return "message";
    case NotificationType::Mention:
        return "mention";
    case NotificationType::System:
        return "system";
    }
    return "system";
}

NotificationType Notification::stringtotype(const std::string &s)
{
    if (s == "like")
    {
        return NotificationType::Like;
    }
    else if (s == "comment")
    {
        return NotificationType::Comment;
    }
    else if (s == "message")
    {
        return NotificationType::Message;
    }
    else if (s == "mention")
    {
        return NotificationType::Mention;
    }
    else if (s == "follow")
    {
        return NotificationType::Follow;
    }
    else if (s == "system")
    {
        return NotificationType::System;
    }
    return NotificationType::System;
}

unsigned long long Notification::getID() const { return n_id; }
unsigned long long Notification::getReciever() const { return r_id; }
unsigned long long Notification::getSender() const { return s_id; }
NotificationType Notification::getType() const { return type; }
std::string Notification::getDescription() const { return desc; }
std::time_t Notification::getTimeStamp() const { return timeStamp; }
bool Notification::getReadStatus() const { return isRead; }

void Notification::markAsRead() { isRead = true; }

json Notification::toJSON() const
{
    return json{
        {"id", n_id},
        {"reciever", r_id},
        {"sender", s_id},
        {"type", typetostring(type)},
        {"description", desc},
        {"timestamp", timeStamp},
        {"readstatus", isRead}};
}

Notification Notification::fromJSON(const json &j)
{
    Notification n;
    n.n_id = j.at("id").get<unsigned long long>();
    n.r_id = j.at("reciever").get<unsigned long long>();
    n.s_id = j.at("sender").get<unsigned long long>();
    n.type = stringtotype(j.at("type").get<std::string>());
    n.desc = j.at("description").get<std::string>();
    n.timeStamp = j.at("timestamp").get<std::time_t>();
    n.isRead = j.at("readstatus").get<bool>();

    if (n.n_id >= nextNID)
    {
        nextNID = n.n_id + 1;
    }

    return n;
}

// Notification Manager class defination

NotificationManager::NotificationManager(const std::string &fp) : cap(10), count(0), head(nullptr), filePath(fp)
{
    list = new Notification *[cap];
}

NotificationManager::NotificationManager(const std::string &fp, unsigned long long initalCap) : cap(initalCap), count(0), head(nullptr), filePath(fp)
{
    if (cap == 0)
    {
        cap = 1;
    }
    list = new Notification *[cap];
}

NotificationManager::~NotificationManager()
{
    for (unsigned long long i = 0; i < count; i++)
    {
        delete list[i];
    }

    delete[] list;

    UnreadNode *curr = head;
    while (curr)
    {
        UnreadNode *next = curr->next;
        delete curr;
        curr = next;
    }
}

void NotificationManager::expand()
{
    unsigned long long newCap = cap * 2;
    Notification **newlist = new Notification *[newCap];

    for (unsigned long long i = 0; i < count; i++)
    {
        newlist[i] = list[i];
    }
    delete[] list;

    list = newlist;
    cap = newCap;
}

void NotificationManager::addToUnread(Notification *n)
{
    UnreadNode *node = new UnreadNode;
    node->data = n;
    node->next = head;
    head = node;
}

void NotificationManager::removeFromUnread(Notification *n)
{
    UnreadNode *curr = head;
    UnreadNode *prev = nullptr;

    while (curr)
    {
        if (curr->data == n)
        {
            if (prev == nullptr)
            {
                head = curr->next;
            }
            else
                prev->next = curr->next;

            delete curr;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

void NotificationManager::add(Notification *n)
{
    if (count == cap)
    {
        expand();
    }

    list[count++] = n;

    if (!n->getReadStatus())
    {
        addToUnread(n);
    }
}
Notification *NotificationManager::create(unsigned long long recieverID, unsigned long long senderID, NotificationType type, const std::string &desc)
{
    Notification *n = new Notification(recieverID, senderID, type, desc);
    add(n);

    return n;
}

Notification *NotificationManager::getByID(unsigned long long nid) const
{
    for (unsigned long long i = 0; i < count; i++)
    {
        if (list[i]->getID() == nid)
        {
            return list[i];
        }
    }
    return nullptr;
}

Notification **NotificationManager::getAll() const
{
    return list;
}

Notification **NotificationManager::getByType(NotificationType t) const
{
    unsigned long long tcount = 0;

    for (unsigned long long i = 0; i < count; i++)
    {
        if (list[i]->getType() == t)
        {
            tcount++;
        }
    }

    if (tcount == 0)
    {
        return nullptr;
    }
    Notification **arr = new Notification *[tcount];

    unsigned long long idx = 0;
    for (unsigned long long i = 0; i < count; i++)
    {
        if (list[i]->getType() == t)
        {
            arr[idx++] = list[i];
        }
    }

    return arr;
}

Notification **NotificationManager::getUnread() const
{
    unsigned long long ucount = countUnread();

    if (ucount == 0)
        return nullptr;

    Notification **arr = new Notification *[ucount];

    UnreadNode *curr = head;
    unsigned long long idx = 0;

    while (curr)
    {
        arr[idx++] = curr->data;
        curr = curr->next;
    }

    return arr;
}

void NotificationManager::markAllRead()
{
    for (unsigned long long i = 0; i < count; i++)
    {
        list[i]->markAsRead();
    }

    UnreadNode *curr = head;
    while (curr)
    {
        UnreadNode *next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
}

void NotificationManager::markAsRead(unsigned long long nid)
{
    Notification *n = getByID(nid);
    if (!n)
        return;

    if (!n->getReadStatus())
    {
        n->markAsRead();
        removeFromUnread(n);
    }
}

bool NotificationManager::removeByID(unsigned long long nid)
{
    for (unsigned long long i = 0; i < count; i++)
    {
        if (list[i]->getID() == nid)
        {
            removeFromUnread(list[i]);

            delete list[i];

            for (unsigned long long j = i; j < count - 1; j++)
            {
                list[j] = list[j + 1];
            }
            count--;

            return true;
        }
    }
    return false;
}

unsigned long long NotificationManager::countUnread() const
{
    unsigned long long c = 0;
    UnreadNode *curr = head;

    while (curr)
    {
        c++;
        curr = curr->next;
    }
    return c;
}

bool NotificationManager::saveToFile() const
{
    std::ofstream out(filePath);
    if (!out.is_open())
    {
        return false;
    }

    json j;
    json arr = json::array();

    for (unsigned long long i = 0; i < count; i++)
    {
        arr.push_back(list[i]->toJSON());
    }

    j["notifications"] = arr;

    out << j.dump(4);
    out.close();
    return true;
}

bool NotificationManager::loadFromFile()
{
    std::ifstream in(filePath);
    if (!in.is_open())
    {
        return false;
    }

    try
    {
        json j;
        in >> j;
        in.close();

        for (unsigned long long i = 0; i < count; i++)
        {
            delete list[i];
        }
        count = 0;

        UnreadNode *curr = head;
        while (curr)
        {
            UnreadNode *next = curr->next;
            delete curr;
            curr = next;
        }
        head = nullptr;

        if (j.contains("notifications") && j["notifications"].is_array())
        {
            for (const auto &item : j["notifications"])
            {
                Notification temp = Notification::fromJSON(item);

                Notification *n = new Notification(temp.getReciever(),
                                                   temp.getSender(),
                                                   temp.getType(),
                                                   temp.getDescription());

                *n = temp;

                add(n);
            }
        }
        return true;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Load Error:  " << e.what() << std::endl;
        return false;
    }
}

bool NotificationManager::empty() const { return count == 0; }
unsigned long long NotificationManager::size() const { return count; }