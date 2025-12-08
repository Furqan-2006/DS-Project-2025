#include "core/user.hpp"

ull User::nextUID = 1000;

User::User(const std::string &uname, const std::string &pwd, const std::string &cty)
    : u_id(nextUID++), uname(uname), password(pwd), city(cty),
      status(State::OFFLINE), lastLoginTime(0), lastLogoutTime(0)
{
}

ull User::getID() const { return u_id; }
std::string User::getUname() const { return uname; }
std::string User::getPassword() const { return password; }
std::string User::getCity() const { return city; }
State User::getStatus() const { return status; }
time_t User::getLastLoginTime() const { return lastLoginTime; }
time_t User::getLastLogoutTime() const { return lastLogoutTime; }

void User::setUname(const std::string &u) { uname = u; }
void User::setPassword(const std::string &p) { password = p; }
void User::setCity(const std::string &c) { city = c; }
void User::setStatus(State s) { status = s; }

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
    if (lastLogoutTime > 0)
    {
        std::cout << "Last Logout: " << ctime(&lastLogoutTime);
    }
}

// UserManager class implementation
UserManager::UserManager() {}

UserManager::~UserManager()
{
    // Use HashMap iterator to delete all User objects
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        delete (*it).value;
    }
    users.clear();
}

bool UserManager::registerUser(const std::string &uname, const std::string &password, const std::string &city)
{
    // Check if user already exists using HashMap::contains
    if (users.contains(uname))
    {
        std::cout << "Error: Username '" << uname << "' already exists.\n";
        return false;
    }

    // Create new user and insert into HashMap
    User *newUser = new User(uname, password, city);
    users.insert(uname, newUser);

    std::cout << "User '" << uname << "' registered successfully with ID: " << newUser->getID() << "\n";
    return true;
}

bool UserManager::loginUser(const std::string &uname, const std::string &password)
{
    // Use HashMap::get to find user
    User **userPtr = users.get(uname);

    if (!userPtr)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return false;
    }

    User *user = *userPtr;

    if (!user->verifyPassword(password))
    {
        std::cout << "Error: Incorrect password.\n";
        return false;
    }

    if (user->getStatus() == State::ONLINE)
    {
        std::cout << "User '" << uname << "' is already logged in.\n";
        return false;
    }

    user->updateLoginTime();
    std::cout << "User '" << uname << "' logged in successfully.\n";
    return true;
}

bool UserManager::logoutUser(const std::string &uname)
{
    // Use HashMap::get to find user
    User **userPtr = users.get(uname);

    if (!userPtr)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return false;
    }

    User *user = *userPtr;

    if (user->getStatus() == State::OFFLINE)
    {
        std::cout << "User '" << uname << "' is already logged out.\n";
        return false;
    }

    user->updateLogoutTime();
    std::cout << "User '" << uname << "' logged out successfully.\n";
    return true;
}

bool UserManager::resetPassword(const std::string &uname, const std::string &newPassword)
{
    // Use HashMap::get to find user
    User **userPtr = users.get(uname);

    if (!userPtr)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return false;
    }

    (*userPtr)->setPassword(newPassword);
    std::cout << "Password reset successfully for user '" << uname << "'.\n";
    return true;
}

bool UserManager::deleteUser(const std::string &uname)
{
    // Use HashMap::get to find user first
    User **userPtr = users.get(uname);

    if (!userPtr)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return false;
    }

    // Delete the User object
    delete *userPtr;

    // Remove from HashMap
    users.remove(uname);

    std::cout << "User '" << uname << "' deleted successfully.\n";
    return true;
}

bool UserManager::updateProfile(const std::string &uname, const std::string newCity, State newStatus)
{
    // Use HashMap::get to find user
    User **userPtr = users.get(uname);

    if (!userPtr)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return false;
    }

    (*userPtr)->setCity(newCity);
    (*userPtr)->setStatus(newStatus);

    std::cout << "Profile updated successfully for user '" << uname << "'.\n";
    return true;
}

User *UserManager::search(const std::string &uname) const
{
    // Use HashMap::get to find user
    User *const *userPtr = users.get(uname);
    return userPtr ? *userPtr : nullptr;
}

std::string UserManager::getStatusForClass(const std::string &uname) const
{
    User *user = search(uname);
    if (!user)
    {
        return "User not found";
    }
    return (user->getStatus() == State::ONLINE) ? "ONLINE" : "OFFLINE";
}

void UserManager::displayAllUsers() const
{
    if (users.size() == 0)
    {
        std::cout << "No users registered.\n";
        return;
    }

    std::cout << "\n===== All Registered Users =====\n";

    // Use HashMap iterator to traverse all users
    for (auto it = users.begin(); it != users.end(); ++it)
    {
        std::cout << "\n";
        (*it).value->display();
        std::cout << "----------------------------\n";
    }
}

void UserManager::displayUserProfile(const std::string &uname) const
{
    // Use search which internally uses HashMap::get
    User *user = search(uname);

    if (!user)
    {
        std::cout << "Error: User '" << uname << "' not found.\n";
        return;
    }

    std::cout << "\n===== User Profile =====\n";
    user->display();
    std::cout << "========================\n";
}