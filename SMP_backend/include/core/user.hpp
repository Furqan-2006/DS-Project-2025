#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include "ADT/hash_map.hpp"

typedef unsigned long long ull;

enum class State
{
    ONLINE,
    OFFLINE
};

class User
{
private:
    ull u_id;

    std::string uname;
    std::string password;
    std::string city;
    State status;

    static ull nextUID;

    time_t lastLoginTime;
    time_t lastLogoutTime;

public:
    User(const std::string &uname, const std::string &pwd, const std::string &cty);

    ull getID() const;
    std::string getUname() const;
    std::string getPassword() const;
    std::string getCity() const;
    State getStatus() const;
    time_t getLastLoginTime() const;
    time_t getLastLogoutTime() const;

    void setUname(const std::string &u);
    void setPassword(const std::string &p);
    void setCity(const std::string &c);
    void setStatus(State s);

    void updateLoginTime();
    void updateLogoutTime();

    bool verifyPassword(const std::string &input) const;
    void display() const;
};

class UserManager
{
private:
    HashMap<std::string, User *> users;

public:
    UserManager();
    ~UserManager();

    bool registerUser(const std::string &uname, const std::string &password, const std::string &city);
    bool loginUser(const std::string &uname, const std::string &password);
    bool logoutUser(const std::string &uname);
    bool resetPassword(const std::string &uname, const std::string &newPassword);

    bool deleteUser(const std::string &uname);
    bool updateProfile(const std::string &uname, const std::string newCity, State newStatus);

    User *search(const std::string &uname) const;
    std::string getStatusForClass(const std::string &uname) const;

    void displayAllUsers() const;
    void displayUserProfile(const std::string &uname) const;

    UserManager(const UserManager &) = delete;
    UserManager &operator=(const UserManager &) = delete;
};
