#include "interaction/message.hpp"
#include "utils/helpers.hpp"
#include <fstream>

unsigned long long Message::nextMID = 1000;

Message::Message()
    : m_id(nextMID++), timeStamp(std::time(nullptr)), isRead(false) {}

Message::Message(const std::string &s, const std::string &r, const std::string &t)
    : m_id(nextMID++), sender(s), reciever(r), text(t), timeStamp(std::time(nullptr)), isRead(false) {}

unsigned long long Message::getID() const { return m_id; }
std::string Message::getSender() const { return sender; }
std::string Message::getReciever() const { return reciever; }
std::string Message::getText() const { return text; }
std::time_t Message::getTimeStamp() const { return timeStamp; }
bool Message::getIsRead() const { return isRead; }

void Message::setID(unsigned long long newID) { m_id = newID; }
void Message::setSender(const std::string &s) { sender = s; }
void Message::setReciever(const std::string &r) { reciever = r; }
void Message::setText(const std::string &t) { text = t; }
void Message::setTimeStamp(std::time_t ts) { timeStamp = ts; }
void Message::setIsRead(bool ir) { isRead = ir; }

void Message::markIsRead() { isRead = true; }

json Message::toJSON() const
{
    return json{
        {"ID", m_id},
        {"sender", sender},
        {"reciever", reciever},
        {"text", text},
        {"timestamp", timeStamp},
        {"isRead", isRead}};
}

Message Message::fromJSON(const json &j)
{
    Message m;
    m.setID(j.at("ID").get<unsigned long long>());
    m.setSender(j.at("sender").get<std::string>());
    m.setReciever(j.at("reciever").get<std::string>());
    m.setText(j.at("text").get<std::string>());
    m.setTimeStamp(j.at("timestamp").get<std::time_t>());
    m.setIsRead(j.at("isRead").get<bool>());

    if (m.getID() >= nextMID)
    {
        nextMID = m.getID() + 1;
    }

    return m;
}

std::string Message::format() const
{
    char buff[26];
    std::strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S", std::localtime(&timeStamp));
    return "[" + std::string(buff) + "] " + sender + " -> " + reciever + ": " + text;
}

// Message System class defination

MessageSystem::MessageSystem(const std::string &path) : filePath(path) {}

void MessageSystem::registerUser(User *user)
{
    if (user)
    {
        users[user->getID()] = user;
    }
}
void MessageSystem::sendMessage(const User &s, const User &r, const std::string &text)
{
    Message msg(s.getUname(), r.getUname(), text);
    std::string key = makeKey(s.getID(), r.getID());

    chat[key].enqueue(msg);
}
std::vector<Message> MessageSystem::getChatHistory(const User &u1, const User &u2) const
{
    std::string key = makeKey(u1.getID(), u2.getID());
    auto it = chat.find(key);

    if (it != chat.end())
    {
        return (*it).value.toVector();
    }
    return {};
}

bool MessageSystem::markMessageRead(const User &u, unsigned long long msgID)
{
    for (auto it = chat.begin(); it != chat.end(); ++it)
    {
        Message *msgPtr = (*it).value.find(msgID);

        if (msgPtr != nullptr && msgPtr->getReciever() == u.getUname())
        {
            msgPtr->markIsRead();
            return true;
        }
    }
    return false;
}

Message MessageSystem::getLastestMessage(const User &u1, const User &u2) const
{
    std::string key = makeKey(u1.getID(), u2.getID());
    auto it = chat.find(key);

    if (it == chat.end() || (*it).value.isEmpty())
    {
        throw std::runtime_error("No Messages found in the Chat!");
    }

    return (*it).value.back();
}

bool MessageSystem::loadFromFile()
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

        clear();

        if (j.contains("chats") && j["chats"].is_object())
        {
            for (const auto &[key, msgArr] : j["chats"].items())
            {
                for (const auto &msgJson : msgArr)
                {

                    Message m = Message::fromJSON(msgJson);
                    chat[key].enqueue(m);
                }
            }
        }
        return true;
    }
    catch (const json::parse_error &e)
    {
        std::cerr << "JSON Parse Error: " << e.what() << std::endl;
        return false;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Load Error: " << e.what() << std::endl;
        return false;
    }
}

bool MessageSystem::saveToFile() const
{
    std::ofstream out(filePath);
    if (!out.is_open())
    {
        return false;
    }
    json j;
    json chatJSON = json::object();

    for (auto it = chat.begin(); it != chat.end(); ++it)
    {
        std::vector<Message> messages = (*it).value.toVector();
        json msgArr = json::array();

        for (const auto &msg : messages)
        {
            msgArr.push_back(msg.toJSON());
        }
        chatJSON[(*it).key] = msgArr;
    }
    j["chats"] = chatJSON;

    out << j.dump(4);
    out.close();
    return true;
}

std::vector<Message> MessageSystem::searchMessages(const User &user, const std::string &keyword) const
{
    std::vector<Message> results;
    std::string lowerKeyword = toLower(keyword);

    for (auto it = chat.begin(); it != chat.end(); ++it)
    {
        std::vector<Message> messages = (*it).value.toVector();

        for (const auto &msg : messages)
        {
            if (msg.getSender() == user.getUname() || msg.getReciever() == user.getUname())
            {
                std::string lowerText = toLower(msg.getText());
                if (lowerText.length() >= lowerKeyword.length())
                {
                    for (size_t i = 0; i <= lowerText.length() - lowerKeyword.length(); ++i)
                    {
                        if (lowerText.substr(i, lowerKeyword.length()) == lowerKeyword)
                        {
                            results.push_back(msg);
                            break;
                        }
                    }
                }
            }
        }
    }
    return results;
}

void MessageSystem::clear()
{
    chat.clear();
    users.clear();
}