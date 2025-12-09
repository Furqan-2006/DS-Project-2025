#include "core/user.hpp"

// ============================================
// User Class Implementation
// ============================================

// Initialize static member
ull User::nextUID = 1;

User::User(const std::string &uname, const std::string &pwd, const std::string &cty)
    : u_id(nextUID++), uname(uname), password(pwd), city(cty),
      status(State::OFFLINE), lastLoginTime(0), lastLogoutTime(0)
{
}

ull User::getID() const
{
    return u_id;
}

std::string User::getUname() const
{
    return uname;
}

std::string User::getPassword() const
{
    return password;
}

std::string User::getCity() const
{
    return city;
}

State User::getStatus() const
{
    return status;
}

time_t User::getLastLoginTime() const
{
    return lastLoginTime;
}

time_t User::getLastLogoutTime() const
{
    return lastLogoutTime;
}

void User::setUname(const std::string &u)
{
    uname = u;
}

void User::setPassword(const std::string &p)
{
    password = p;
}

void User::setCity(const std::string &c)
{
    city = c;
}

void User::setStatus(State s)
{
    status = s;
}

void User::updateLoginTime()
{
    lastLoginTime = time(nullptr);
    status = State::ONLINE;
}

void User::updateLogoutTime()
{
    lastLogoutTime = time(nullptr);
    status = State::OFFLINE;
}

bool User::verifyPassword(const std::string &input) const
{
    return password == input;
}

void User::display() const
{
    std::cout << "User ID: " << u_id << "\n";
    std::cout << "Username: " << uname << "\n";
    std::cout << "City: " << city << "\n";
    std::cout << "Status: " << (status == State::ONLINE ? "ONLINE" : "OFFLINE") << "\n";

    if (lastLoginTime > 0)
    {
        std::cout << "Last Login: " << ctime(&lastLoginTime);
    }
    else
    {
        std::cout << "Last Login: Never\n";
    }

    if (lastLogoutTime > 0)
    {
        std::cout << "Last Logout: " << ctime(&lastLogoutTime);
    }
    else
    {
        std::cout << "Last Logout: Never\n";
    }
}

// ============================================
// UserManager Class Implementation
// ============================================

UserManager::UserManager() : usersByID(100), usernameToID(100)
{
}

UserManager::~UserManager()
{
    // Clean up all user objects
    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        delete (*it).value;
    }
}

bool UserManager::registerUser(const std::string &uname, const std::string &password, const std::string &city)
{
    // Validate inputs
    if (!validator.isValidUsername(uname))
    {
        std::cout << "Invalid username. Requirements:\n";
        std::cout << "  - 3-20 characters\n";
        std::cout << "  - Only alphanumeric, underscore, and dot allowed\n";
        return false;
    }

    if (!validator.isValidPassword(password))
    {
        std::cout << "Invalid password. Requirements:\n";
        std::cout << "  - 6-50 characters\n";
        std::cout << "  - At least one uppercase letter\n";
        std::cout << "  - At least one lowercase letter\n";
        std::cout << "  - At least one digit\n";
        return false;
    }

    if (!validator.isValidCity(city))
    {
        std::cout << "Invalid city. Requirements:\n";
        std::cout << "  - 2-50 characters\n";
        std::cout << "  - Only letters, spaces, hyphens, and apostrophes allowed\n";
        return false;
    }

    // Check if username already exists
    if (usernameToID.contains(uname))
    {
        std::cout << "Username already exists.\n";
        return false;
    }

    // Create new user
    User *newUser = new User(uname, password, city);
    ull userID = newUser->getID();

    // Insert into both hash maps
    usersByID.insert(userID, newUser);
    usernameToID.insert(uname, userID);

    std::cout << "User registered successfully with ID: " << userID << "\n";
    return true;
}

bool UserManager::loginUser(const std::string &uname, const std::string &password)
{
    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return false;
    }

    if (!user->verifyPassword(password))
    {
        std::cout << "Incorrect password.\n";
        return false;
    }

    if (user->getStatus() == State::ONLINE)
    {
        std::cout << "User is already logged in.\n";
        return false;
    }

    user->updateLoginTime();
    std::cout << "Login successful. Welcome, " << uname << "!\n";
    return true;
}

bool UserManager::logoutUser(const std::string &uname)
{
    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return false;
    }

    if (user->getStatus() == State::OFFLINE)
    {
        std::cout << "User is already logged out.\n";
        return false;
    }

    user->updateLogoutTime();
    std::cout << "Logout successful. Goodbye, " << uname << "!\n";
    return true;
}

bool UserManager::resetPassword(const std::string &uname, const std::string &newPassword)
{
    if (!validator.isValidPassword(newPassword))
    {
        std::cout << "Invalid password. Requirements:\n";
        std::cout << "  - 6-50 characters\n";
        std::cout << "  - At least one uppercase letter\n";
        std::cout << "  - At least one lowercase letter\n";
        std::cout << "  - At least one digit\n";
        return false;
    }

    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return false;
    }

    user->setPassword(newPassword);
    std::cout << "Password reset successful for user: " << uname << "\n";
    return true;
}

bool UserManager::deleteUser(const std::string &uname)
{
    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return false;
    }

    ull userID = user->getID();

    // Remove from both hash maps
    usernameToID.remove(uname);
    usersByID.remove(userID);

    // Delete the user object
    delete user;

    std::cout << "User deleted successfully: " << uname << "\n";
    return true;
}

bool UserManager::updateProfile(const std::string &uname, const std::string &newCity, State newStatus)
{
    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return false;
    }

    if (!validator.isValidCity(newCity))
    {
        std::cout << "Invalid city. Requirements:\n";
        std::cout << "  - 2-50 characters\n";
        std::cout << "  - Only letters, spaces, hyphens, and apostrophes allowed\n";
        return false;
    }

    user->setCity(newCity);
    user->setStatus(newStatus);

    std::cout << "Profile updated successfully for user: " << uname << "\n";
    return true;
}

User *UserManager::getUserByID(ull userID) const
{
    User *const *userPtr = usersByID.get(userID);
    return userPtr ? *userPtr : nullptr;
}

User *UserManager::getUserByUsername(const std::string &uname) const
{
    const ull *userIDPtr = usernameToID.get(uname);
    if (!userIDPtr)
    {
        return nullptr;
    }

    return getUserByID(*userIDPtr);
}

ull UserManager::getUserIDByUsername(const std::string &uname) const
{
    const ull *userIDPtr = usernameToID.get(uname);
    return userIDPtr ? *userIDPtr : 0;
}

DynamicArray<User *> UserManager::searchByCity(const std::string &city) const
{
    DynamicArray<User *> results;

    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        User *user = (*it).value;
        if (user->getCity() == city)
        {
            results.push_back(user);
        }
    }

    return results;
}

DynamicArray<User *> UserManager::searchByStatus(State status) const
{
    DynamicArray<User *> results;

    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        User *user = (*it).value;
        if (user->getStatus() == status)
        {
            results.push_back(user);
        }
    }

    return results;
}

DynamicArray<User *> UserManager::getAllUsers() const
{
    DynamicArray<User *> results;

    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        results.push_back((*it).value);
    }

    return results;
}

void UserManager::displayAllUsers() const
{
    if (usersByID.size() == 0)
    {
        std::cout << "No users registered.\n";
        return;
    }

    std::cout << "\n========== All Users ==========\n";
    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        User *user = (*it).value;
        std::cout << "\n";
        user->display();
        std::cout << "-------------------------------\n";
    }
}

void UserManager::displayUserProfile(const std::string &uname) const
{
    User *user = getUserByUsername(uname);

    if (!user)
    {
        std::cout << "User not found.\n";
        return;
    }

    std::cout << "\n========== User Profile ==========\n";
    user->display();
    std::cout << "==================================\n";
}

void UserManager::displayUserProfileByID(ull userID) const
{
    User *user = getUserByID(userID);

    if (!user)
    {
        std::cout << "User not found.\n";
        return;
    }

    std::cout << "\n========== User Profile ==========\n";
    user->display();
    std::cout << "==================================\n";
}

int UserManager::getTotalUsers() const
{
    return static_cast<int>(usersByID.size());
}

int UserManager::getOnlineUsersCount() const
{
    int count = 0;
    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        if ((*it).value->getStatus() == State::ONLINE)
        {
            count++;
        }
    }
    return count;
}

int UserManager::getOfflineUsersCount() const
{
    int count = 0;
    for (auto it = usersByID.begin(); it != usersByID.end(); ++it)
    {
        if ((*it).value->getStatus() == State::OFFLINE)
        {
            count++;
        }
    }
    return count;
}

bool UserManager::validateUsername(const std::string &uname) const
{
    return validator.isValidUsername(uname);
}

bool UserManager::validatePassword(const std::string &pwd) const
{
    return validator.isValidPassword(pwd);
}

bool UserManager::validateCity(const std::string &city) const
{
    return validator.isValidCity(city);
}