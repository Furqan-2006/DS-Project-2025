#pragma once

#include <iostream>
#include <string>
#include <ctime>
#include "ADT/hash_map.hpp"
#include "utils/validation.hpp"

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

// Simple dynamic array to avoid STL vector
template <typename T>
class DynamicArray
{
private:
    T *data;
    int capacity;
    int size;

    void resize()
    {
        capacity = capacity == 0 ? 1 : capacity * 2;
        T *newData = new T[capacity];
        for (int i = 0; i < size; ++i)
        {
            newData[i] = data[i];
        }
        delete[] data;
        data = newData;
    }

public:
    DynamicArray() : data(nullptr), capacity(0), size(0) {}

    ~DynamicArray()
    {
        delete[] data;
    }

    void push_back(const T &value)
    {
        if (size == capacity)
        {
            resize();
        }
        data[size++] = value;
    }

    T &operator[](int index)
    {
        return data[index];
    }

    const T &operator[](int index) const
    {
        return data[index];
    }

    int getSize() const
    {
        return size;
    }

    bool isEmpty() const
    {
        return size == 0;
    }

    void clear()
    {
        size = 0;
    }
};

class UserManager
{
private:
    // Primary DS: Hash Table (userID → User*)
    HashMap<ull, User *> usersByID;

    // Secondary index: username → userID for O(1) username lookups
    HashMap<std::string, ull> usernameToID;

    // Validator for string validation algorithms
    UserValidator validator;

public:
    UserManager();
    ~UserManager();

    // Core Responsibilities
    bool registerUser(const std::string &uname, const std::string &password, const std::string &city);
    bool loginUser(const std::string &uname, const std::string &password);
    bool logoutUser(const std::string &uname);
    bool resetPassword(const std::string &uname, const std::string &newPassword);
    bool deleteUser(const std::string &uname);
    bool updateProfile(const std::string &uname, const std::string &newCity, State newStatus);

    // Expose user entities to other managers (via System Manager)
    User *getUserByID(ull userID) const;
    User *getUserByUsername(const std::string &uname) const;
    ull getUserIDByUsername(const std::string &uname) const;

    // Linear search for scanning profile fields
    DynamicArray<User *> searchByCity(const std::string &city) const;
    DynamicArray<User *> searchByStatus(State status) const;
    DynamicArray<User *> getAllUsers() const;

    // Display functions
    void displayAllUsers() const;
    void displayUserProfile(const std::string &uname) const;
    void displayUserProfileByID(ull userID) const;

    // Statistics
    int getTotalUsers() const;
    int getOnlineUsersCount() const;
    int getOfflineUsersCount() const;

    // Validation exposure
    bool validateUsername(const std::string &uname) const;
    bool validatePassword(const std::string &pwd) const;
    bool validateCity(const std::string &city) const;

    UserManager(const UserManager &) = delete;
    UserManager &operator=(const UserManager &) = delete;
};