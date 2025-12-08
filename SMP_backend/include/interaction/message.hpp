#pragma once

#include <string>
#include <ctime>
#include <vector>
#include "ADT/hash_map.hpp"
#include "ADT/queue.hpp"
#include "nlohmann/json.hpp"
#include "core/user.hpp"

using json = nlohmann::json;

class Message
{
private:
    unsigned long long m_id;

    std::string sender;
    std::string reciever;
    std::string text;
    std::time_t timeStamp;
    bool isRead;

    static unsigned long long nextMID;

public:
    Message();
    Message(const std::string &s, const std::string &r, const std::string &t);

    unsigned long long getID() const;
    std::string getSender() const;
    std::string getReciever() const;
    std::string getText() const;
    std::time_t getTimeStamp() const;
    bool getIsRead() const;

    void setID(unsigned long long newID);
    void setSender(const std::string &s);
    void setReciever(const std::string &r);
    void setText(const std::string &t);
    void setTimeStamp(std::time_t ts);
    void setIsRead(bool ir);

    void markIsRead();

    json toJSON() const;
    static Message fromJSON(const json &j);

    std::string format() const;

    bool operator==(const Message &other) const { return this->m_id == other.m_id; }
};

class MessageSystem
{
private:
    HashMap<std::string, Queue<Message>> chat;
    HashMap<unsigned long long, User *> users;

    std::string filePath;

    std::string makeKey(unsigned long long a, unsigned long long b) const
    {
        return (a < b) ? std::to_string(a) + "_" + std::to_string(b)
                       : std::to_string(b) + "_" + std::to_string(a);
    }

public:
    MessageSystem(const std::string &path);

    void registerUser(User *user);

    void sendMessage(const User &sender, const User &reciever, const std::string &text);

    std::vector<Message> getChatHistory(const User &u1, const User &u2) const;

    bool markMessageRead(const User &user, unsigned long long messageID);

    Message getLastestMessage(const User &u1, const User &u2) const;

    bool loadFromFile();
    bool saveToFile() const;

    std::vector<Message> searchMessages(const User &user, const std::string &keyword) const;
    void clear();
};