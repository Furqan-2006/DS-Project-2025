#include "system/SystemManager.hpp"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

// Color codes for better UI
#define RESET "\033[0m"
#define BOLD "\033[1m"
#define RED "\033[31m"
#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"

class SocialPlatformCLI
{
private:
    SystemManager *sysManager;
    ull currentUserID;
    std::string currUserName;
    bool isLoggedIn;

    void clearScreen()
    {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
    }

    void pause()
    {
        std::cout << "\n"
                  << YELLOW << "Press Enter to continue..." << RESET;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cin.get();
    }

    void printHeader(const std::string &title)
    {
        std::cout << "\n"
                  << BOLD << CYAN;
        std::cout << "/------------------------------------------------------------\\\n";
        std::cout << "| " << std::setw(58) << std::left << title << " |\n";
        std::cout << "\\------------------------------------------------------------/\n";
        std::cout << RESET;
    }

    void printSuccess(const std::string &msg)
    {
        std::cout << GREEN << "200 " << msg << RESET << "\n";
    }

    void printError(const std::string &msg)
    {
        std::cout << RED << "404 " << msg << RESET << "\n";
    }

    void printInfo(const std::string &msg)
    {
        std::cout << BLUE << "i " << msg << RESET << "\n";
    }

    std::string getInput(const std::string &prompt)
    {
        std::cout << BOLD << prompt << RESET;
        std::string input;
        std::getline(std::cin, input);
        return input;
    }

    int getChoice(const std::string &prompt, int min, int max)
    {
        int choice;
        while (true)
        {
            std::cout << BOLD << prompt << RESET;
            if (std::cin >> choice && choice >= min && choice <= max)
            {
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return choice;
            }
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            printError("Invalid choice. Please try again.");
        }
    }

    void displayMainMenu()
    {
        printHeader("SOCIAL PLATFORM - MAIN MENU");
        std::cout << "\n";
        std::cout << "  " << BOLD << "1." << RESET << " Register New Account\n";
        std::cout << "  " << BOLD << "2." << RESET << " Login\n";
        std::cout << "  " << BOLD << "3." << RESET << " View All Users\n";
        std::cout << "  " << BOLD << "4." << RESET << " System Status\n";
        std::cout << "  " << BOLD << "0." << RESET << " Exit\n";
        std::cout << "\n";
    }

    void displayUserMenu()
    {
        User *user = sysManager->getUser(currentUserID);
        printHeader("DASHBOARD - Welcome, " + user->getUname() + "!");

        // Show quick stats
        auto stats = sysManager->getUserStats(currentUserID);
        std::cout << "\n"
                  << BOLD << "Quick Stats:" << RESET;
        std::cout << " Posts: " << stats.postCount;
        std::cout << " | Friends: " << stats.friendCount;
        std::cout << " | Unread: " << stats.unreadNotifications;
        std::cout << " | " << (stats.isOnline ? GREEN "●" : RED "○") << " " << (stats.isOnline ? "Online" : "Offline") << RESET << "\n\n";

        std::cout << "  " << BOLD << "1." << RESET << " My Profile\n";
        std::cout << "  " << BOLD << "2." << RESET << " Create Post\n";
        std::cout << "  " << BOLD << "3." << RESET << " View Feed\n";
        std::cout << "  " << BOLD << "4." << RESET << " My Posts\n";
        std::cout << "  " << BOLD << "5." << RESET << " Search Users\n";
        std::cout << "  " << BOLD << "6." << RESET << " Search Posts\n";
        std::cout << "  " << BOLD << "7." << RESET << " Notifications (" << stats.unreadNotifications << ")\n";
        std::cout << "  " << BOLD << "8." << RESET << " Messages\n";
        std::cout << "  " << BOLD << "9." << RESET << " Friend Requests (" << stats.pendingRequests << ")\n";
        std::cout << "  " << BOLD << "10." << RESET << " Manage Connections\n";
        std::cout << "  " << BOLD << "11." << RESET << " Recommendations\n";
        std::cout << "  " << BOLD << "12." << RESET << " Settings\n";
        std::cout << "  " << BOLD << "0." << RESET << " Logout\n";
        std::cout << "\n";
    }

    void registerAccount()
    {
        clearScreen();
        printHeader("REGISTER NEW ACCOUNT");

        std::string username = getInput("\nUsername: ");
        std::string password = getInput("Password: ");
        std::string city = getInput("City: ");

        if (sysManager->registerUser(username, password, city))
        {
            printSuccess("Account created successfully!");
            printInfo("You can now login with your credentials.");
        }
        else
        {
            printError("Registration failed. Username may already exist.");
        }
        pause();
    }

    void login()
    {
        clearScreen();
        printHeader("LOGIN");

        std::string username = getInput("\nUsername: ");
        std::string password = getInput("Password: ");

        User *user = sysManager->getUserByUsername(username);
        if (!user)
        {
            printError("User not found!");
            pause();
            return;
        }

        auto result = sysManager->performCompleteLogin(user->getID(), password);
        if (result.success)
        {
            currentUserID = user->getID();
            currUserName = user->getUname();
            isLoggedIn = true;
            printSuccess("Login successful!");

            if (!result.unreadNotifications.empty())
            {
                std::cout << "\n"
                          << YELLOW << "📬 You have " << result.unreadNotifications.size()
                          << " unread notification(s)!" << RESET << "\n";
            }

            if (!result.onlineFriends.empty())
            {
                std::cout << GREEN << "👥 " << result.onlineFriends.size()
                          << " friend(s) are online" << RESET << "\n";
            }
        }
        else
        {
            printError("Login failed. Invalid credentials.");
        }
        pause();
    }

    void viewProfile()
    {
        clearScreen();
        sysManager->displayUserProfile(currentUserID);
        pause();
    }

    void createPost()
    {
        clearScreen();
        printHeader("CREATE NEW POST");

        std::cout << "\nEnter your post content (or 'cancel' to go back):\n";
        std::string content = getInput("> ");

        if (content == "cancel" || content.empty())
        {
            printInfo("Post cancelled.");
            pause();
            return;
        }

        Post *post = sysManager->performCreatePostComplete(currentUserID, content);
        if (post)
        {
            printSuccess("Post created successfully!");
            std::cout << "Post ID: " << post->getPostID() << "\n";
        }
        else
        {
            printError("Failed to create post.");
        }
        pause();
    }

    void viewFeed()
    {
        clearScreen();
        printHeader("YOUR FEED");

        int choice = getChoice("\n1. All Feed\n2. Friends Only\n3. Trending\n\nChoice: ", 1, 3);

        size_t limit = 20;
        std::vector<Post *> feed;

        switch (choice)
        {
        case 1:
            feed = sysManager->getUserFeed(currentUserID, limit);
            break;
        case 2:
            feed = sysManager->getUserFeedByFriends(currentUserID, limit);
            break;
        case 3:
            feed = sysManager->getTrendingPosts(limit);
            break;
        }

        if (feed.empty())
        {
            printInfo("No posts to display.");
        }
        else
        {
            std::cout << "\n"
                      << BOLD << "Showing " << feed.size() << " post(s):\n"
                      << RESET;
            for (Post *post : feed)
            {
                if (post)
                {
                    std::cout << "\n"
                              << CYAN << "───────────────────────────────────────" << RESET << "\n";
                    post->display();
                    std::cout << "  💙 " << post->getLikesCount() << " likes";
                    if (sysManager->hasUserLikedPost(currentUserID, post->getPostID()))
                    {
                        std::cout << GREEN << " (You liked this)" << RESET;
                    }
                    std::cout << "\n";
                }
            }
        }
        pause();
    }

    void viewMyPosts()
    {
        clearScreen();
        printHeader("MY POSTS");

        auto posts = sysManager->getUserPosts(currentUserID);
        if (posts.empty())
        {
            printInfo("You haven't created any posts yet.");
        }
        else
        {
            std::cout << "\n"
                      << BOLD << "You have " << posts.size() << " post(s):\n"
                      << RESET;
            for (Post *post : posts)
            {
                if (post)
                {
                    std::cout << "\n"
                              << CYAN << "---------------------------------------" << RESET << "\n";
                    post->display();
                    std::cout << "  💙 " << post->getLikesCount() << " likes\n";
                }
            }

            std::cout << "\n";
            int action = getChoice("1. Like a post\n2. Delete a post\n0. Back\n\nChoice: ", 0, 2);

            if (action == 1)
            {
                ull postID = getChoice("\nEnter Post ID to like: ", 0, 999999);
                interactWithPost(postID);
            }
            else if (action == 2)
            {
                ull postID = getChoice("\nEnter Post ID to delete: ", 0, 999999);
                if (sysManager->deletePost(postID))
                {
                    printSuccess("Post deleted successfully!");
                }
                else
                {
                    printError("Failed to delete post.");
                }
            }
        }
        pause();
    }

    void searchUsers()
    {
        clearScreen();
        printHeader("SEARCH USERS");

        std::string query = getInput("\nEnter username to search: ");
        if (query.empty())
        {
            printInfo("Search cancelled.");
            pause();
            return;
        }

        auto results = sysManager->searchUsersPrefix(query);
        if (results.empty())
        {
            printInfo("No users found matching '" + query + "'");
        }
        else
        {
            std::cout << "\n"
                      << BOLD << "Found " << results.size() << " user(s):\n"
                      << RESET;
            for (size_t i = 0; i < results.size(); ++i)
            {
                User *u = results[i];
                std::cout << "\n"
                          << (i + 1) << ". ";
                std::cout << BOLD << u->getUname() << RESET;
                std::cout << " (ID: " << u->getID() << ")";
                std::cout << " | " << u->getCity();
                if (sysManager->isUserOnline(u->getID()))
                {
                    std::cout << " " << GREEN << "●" << RESET;
                }
                std::cout << "\n";
            }

            std::cout << "\n";
            int action = getChoice("1. View profile\n2. Follow\n3. Send friend request\n0. Back\n\nChoice: ", 0, 3);

            if (action > 0 && action <= 3)
            {
                ull userID = getChoice("\nEnter User ID: ", 0, 999999);
                handleUserAction(userID, action);
            }
        }
        pause();
    }

    void handleUserAction(ull targetUserID, int action)
    {
        if (!sysManager->validateUser(targetUserID))
        {
            printError("User not found!");
            return;
        }

        switch (action)
        {
        case 1: // View profile
            sysManager->displayUserProfile(targetUserID);
            break;
        case 2: // Follow
            if (sysManager->performFollowWithNotification(currentUserID, targetUserID))
            {
                printSuccess("You are now following this user!");
            }
            else
            {
                printError("Failed to follow user.");
            }
            break;
        case 3: // Send friend request
            if (sysManager->performSendFriendRequestComplete(currentUserID, targetUserID))
            {
                printSuccess("Friend request sent!");
            }
            else
            {
                printError("Failed to send friend request.");
            }
            break;
        }
    }

    void searchPosts()
    {
        clearScreen();
        printHeader("SEARCH POSTS");

        std::string query = getInput("\nEnter search query: ");
        if (query.empty())
        {
            printInfo("Search cancelled.");
            pause();
            return;
        }

        auto results = sysManager->searchPostsPrefix(query);
        if (results.empty())
        {
            printInfo("No posts found matching '" + query + "'");
        }
        else
        {
            std::cout << "\n"
                      << BOLD << "Found " << results.size() << " post(s):\n"
                      << RESET;
            for (Post *post : results)
            {
                if (post)
                {
                    std::cout << "\n"
                              << CYAN << "---------------------------------------" << RESET << "\n";
                    post->display();
                    std::cout << "  💙 " << post->getLikesCount() << " likes\n";
                }
            }

            std::cout << "\n";
            int action = getChoice("1. Like a post\n0. Back\n\nChoice: ", 0, 1);

            if (action == 1)
            {
                ull postID = getChoice("\nEnter Post ID: ", 0, 999999);
                interactWithPost(postID);
            }
        }
        pause();
    }

    void interactWithPost(ull postID)
    {
        if (!sysManager->validatePost(postID))
        {
            printError("Post not found!");
            return;
        }

        if (sysManager->hasUserLikedPost(currentUserID, postID))
        {
            int choice = getChoice("You already liked this post. Unlike? (1=Yes, 0=No): ", 0, 1);
            if (choice == 1)
            {
                if (sysManager->unlikePost(currentUserID, postID))
                {
                    printSuccess("Post unliked.");
                }
            }
        }
        else
        {
            if (sysManager->performLikePostComplete(currentUserID, postID))
            {
                printSuccess("Post liked!");
            }
            else
            {
                printError("Failed to like post.");
            }
        }
    }

    void viewNotifications()
    {
        clearScreen();
        printHeader("NOTIFICATIONS");

        auto notifications = sysManager->getAllNotifications(currentUserID);
        if (notifications.empty())
        {
            printInfo("No notifications.");
        }
        else
        {
            std::cout << "\n";
            for (Notification *notif : notifications)
            {
                if (notif)
                {
                    if (!notif->getReadStatus())
                    {
                        std::cout << YELLOW << "🔔 [NEW] " << RESET;
                    }
                    else
                    {
                        std::cout << "    ";
                    }
                    std::cout << notif->getDescription() << "\n";
                }
            }

            std::cout << "\n";
            int action = getChoice("1. Mark all as read\n0. Back\n\nChoice: ", 0, 1);

            if (action == 1)
            {
                sysManager->markAllNotificationsAsRead(currentUserID);
                printSuccess("All notifications marked as read.");
            }
        }
        pause();
    }

    void manageMessages()
    {
        clearScreen();
        printHeader("MESSAGES");

        ull recipientID = getChoice("\nEnter User ID to message (0 to cancel): ", 0, 999999);
        if (recipientID == 0)
        {
            return;
        }

        if (!sysManager->validateUser(recipientID))
        {
            printError("User not found!");
            pause();
            return;
        }

        // Show chat history
        auto history = sysManager->getMessageHistory(currentUserID, recipientID);
        if (!history.empty())
        {
            std::cout << "\n"
                      << BOLD << "Chat History:\n"
                      << RESET;
            for (const Message &msg : history)
            {
                if (msg.getSender() == currUserName)
                {
                    std::cout << GREEN << "You: " << RESET;
                }
                else
                {
                    std::cout << BLUE << "Them: " << RESET;
                }
                std::cout << msg.getText() << "\n";
            }
        }

        std::cout << "\n";
        std::string content = getInput("Type your message (or 'cancel'): ");

        if (content != "cancel" && !content.empty())
        {
            if (sysManager->performSendMessageComplete(currentUserID, recipientID, content))
            {
                printSuccess("Message sent!");
            }
            else
            {
                printError("Failed to send message.");
            }
        }
        pause();
    }

    void manageFriendRequests()
    {
        clearScreen();
        printHeader("FRIEND REQUESTS");

        auto incoming = sysManager->getPendingIncomingRequests(currentUserID);
        auto outgoing = sysManager->getPendingOutgoingRequests(currentUserID);

        std::cout << "\n"
                  << BOLD << "Incoming Requests (" << incoming.size() << "):\n"
                  << RESET;
        if (incoming.empty())
        {
            std::cout << "  No incoming requests.\n";
        }
        else
        {
            for (const auto &req : incoming)
            {
                std::cout << "  • From User ID: " << req.getSenderID() << "\n";
            }
        }

        std::cout << "\n"
                  << BOLD << "Outgoing Requests (" << outgoing.size() << "):\n"
                  << RESET;
        if (outgoing.empty())
        {
            std::cout << "  No outgoing requests.\n";
        }
        else
        {
            for (const auto &req : outgoing)
            {
                std::cout << "  • To User ID: " << req.getRecieverID() << "\n";
            }
        }

        if (!incoming.empty())
        {
            std::cout << "\n";
            int action = getChoice("1. Accept request\n2. Reject request\n0. Back\n\nChoice: ", 0, 2);

            if (action > 0)
            {
                ull senderID = getChoice("\nEnter Sender ID: ", 0, 999999);
                if (action == 1)
                {
                    if (sysManager->performAcceptFriendRequest(currentUserID, senderID))
                    {
                        printSuccess("Friend request accepted!");
                    }
                    else
                    {
                        printError("Failed to accept request.");
                    }
                }
                else
                {
                    if (sysManager->rejectFriendRequest(currentUserID, senderID))
                    {
                        printSuccess("Friend request rejected.");
                    }
                    else
                    {
                        printError("Failed to reject request.");
                    }
                }
            }
        }
        pause();
    }

    void manageConnections()
    {
        clearScreen();
        printHeader("MANAGE CONNECTIONS");

        std::cout << "\n";
        std::cout << "  1. View Followers\n";
        std::cout << "  2. View Following\n";
        std::cout << "  3. View Friends\n";
        std::cout << "  4. Unfollow User\n";
        std::cout << "  0. Back\n";
        std::cout << "\n";

        int choice = getChoice("Choice: ", 0, 4);

        switch (choice)
        {
        case 1:
            displayFollowers();
            break;
        case 2:
            displayFollowing();
            break;
        case 3:
            displayFriends();
            break;
        case 4:
        {
            ull targetID = getChoice("\nEnter User ID to unfollow: ", 0, 999999);
            if (sysManager->performUnfollowComplete(currentUserID, targetID))
            {
                printSuccess("Unfollowed successfully.");
            }
            else
            {
                printError("Failed to unfollow.");
            }
        }
        break;
        }
        pause();
    }

    void displayFollowers()
    {
        auto followers = sysManager->getFollowers(currentUserID);
        std::cout << "\n"
                  << BOLD << "Your Followers (" << followers.size() << "):\n"
                  << RESET;
        for (ull id : followers)
        {
            User *u = sysManager->getUser(id);
            if (u)
            {
                std::cout << "  • " << u->getUname() << " (ID: " << id << ")\n";
            }
        }
    }

    void displayFollowing()
    {
        auto following = sysManager->getFollowing(currentUserID);
        std::cout << "\n"
                  << BOLD << "You're Following (" << following.size() << "):\n"
                  << RESET;
        for (ull id : following)
        {
            User *u = sysManager->getUser(id);
            if (u)
            {
                std::cout << "  • " << u->getUname() << " (ID: " << id << ")\n";
            }
        }
    }

    void displayFriends()
    {
        auto friends = sysManager->getFriends(currentUserID);
        std::cout << "\n"
                  << BOLD << "Your Friends (" << friends.size() << "):\n"
                  << RESET;
        for (ull id : friends)
        {
            User *u = sysManager->getUser(id);
            if (u)
            {
                std::cout << "  • " << u->getUname() << " (ID: " << id << ")";
                if (sysManager->isUserOnline(id))
                {
                    std::cout << " " << GREEN << "●" << RESET;
                }
                std::cout << "\n";
            }
        }
    }

    void viewRecommendations()
    {
        clearScreen();
        printHeader("RECOMMENDATIONS");

        std::cout << "\n"
                  << BOLD << "Recommended Users:\n"
                  << RESET;
        auto users = sysManager->getRecommendedUsers(currentUserID, 5);
        if (users.empty())
        {
            std::cout << "  No recommendations available.\n";
        }
        else
        {
            for (ull id : users)
            {
                User *u = sysManager->getUser(id);
                if (u)
                {
                    std::cout << "  • " << u->getUname() << " (ID: " << id << ")\n";
                }
            }
        }

        std::cout << "\n"
                  << BOLD << "Recommended Posts:\n"
                  << RESET;
        auto posts = sysManager->getRecommendedPosts(currentUserID, 5);
        if (posts.empty())
        {
            std::cout << "  No recommendations available.\n";
        }
        else
        {
            for (Post *post : posts)
            {
                if (post)
                {
                    std::cout << "\n"
                              << CYAN << "------------------------------------" << RESET << "\n";
                    post->display();
                    std::cout << "  💙 " << post->getLikesCount() << " likes\n";
                }
            }
        }
        pause();
    }

    void settings()
    {
        clearScreen();
        printHeader("SETTINGS");

        std::cout << "\n";
        std::cout << "  1. Update Profile\n";
        std::cout << "  2. Change Password\n";
        std::cout << "  3. Delete Account\n";
        std::cout << "  0. Back\n";
        std::cout << "\n";

        int choice = getChoice("Choice: ", 0, 3);

        switch (choice)
        {
        case 1:
        {
            std::string newCity = getInput("\nEnter new city: ");
            if (sysManager->updateUserProfile(currentUserID, newCity))
            {
                printSuccess("Profile updated successfully!");
            }
            else
            {
                printError("Failed to update profile.");
            }
        }
        break;
        case 2:
        {
            std::string newPassword = getInput("\nEnter new password: ");
            if (sysManager->resetPassword(currentUserID, newPassword))
            {
                printSuccess("Password changed successfully!");
            }
            else
            {
                printError("Failed to change password.");
            }
        }
        break;
        case 3:
        {
            std::cout << "\n"
                      << RED << BOLD << "⚠ WARNING: This action cannot be undone!" << RESET << "\n";
            int confirm = getChoice("Are you sure? (1=Yes, 0=No): ", 0, 1);
            if (confirm == 1)
            {
                if (sysManager->deleteUser(currentUserID))
                {
                    printSuccess("Account deleted successfully.");
                    isLoggedIn = false;
                    currentUserID = 0;
                }
                else
                {
                    printError("Failed to delete account.");
                }
            }
        }
        break;
        }
        pause();
    }

public:
    SocialPlatformCLI()
    {
        sysManager = new SystemManager();
        currentUserID = 0;
        isLoggedIn = false;
    }

    ~SocialPlatformCLI()
    {
        delete sysManager;
    }

    void run()
    {
        clearScreen();
        std::cout << BOLD << MAGENTA;
        std::cout << R"(
    __  __ _       _   ___           _                                  
   |  \/  (_)_ __ (_) |_ _|_ __  ___| |_ __ _  __ _ _ __ __ _ _ __ ___  
   | |\/| | | '_ \| |  | || '_ \/ __| __/ _` |/ _` | '__/ _` | '_ ` _  ` 
   | |  | | | | | | |  | || | | \__ \ || (_| | (_| | | | (_| | | | | | |
   |_|  |_|_|_| |_|_| |___|_| |_|___/\__\__,_|\__, |_|  \__,_|_| |_| |_|
                                               |___/                      
        )" << RESET
                  << "\n";

        printInfo("Initializing system...");
        if (!sysManager->initializeAllModules())
        {
            printError("Failed to initialize system completely. Some features may not work.");
        }
        else
        {
            printSuccess("System initialized successfully!");
        }
        pause();

        while (true)
        {
            clearScreen();

            if (!isLoggedIn)
            {
                displayMainMenu();
                int choice = getChoice("Enter your choice: ", 0, 4);

                switch (choice)
                {
                case 1:
                    registerAccount();
                    break;
                case 2:
                    login();
                    break;
                case 3:
                    clearScreen();
                    sysManager->displayAllUsers();
                    pause();
                    break;
                case 4:
                    clearScreen();
                    sysManager->displaySystemStatus();
                    pause();
                    break;
                case 0:
                    printInfo("Shutting down system...");
                    sysManager->shutdownAllModules();
                    printSuccess("Goodbye!");
                    return;
                }
            }
            else
            {
                displayUserMenu();
                int choice = getChoice("Enter your choice: ", 0, 12);

                switch (choice)
                {
                case 1:
                    viewProfile();
                    break;
                case 2:
                    createPost();
                    break;
                case 3:
                    viewFeed();
                    break;
                case 4:
                    viewMyPosts();
                    break;
                case 5:
                    searchUsers();
                    break;
                case 6:
                    searchPosts();
                    break;
                case 7:
                    viewNotifications();
                    break;
                case 8:
                    manageMessages();
                    break;
                case 9:
                    manageFriendRequests();
                    break;
                case 10:
                    manageConnections();
                    break;
                case 11:
                    viewRecommendations();
                    break;
                case 12:
                    settings();
                    if (!isLoggedIn) // Account might have been deleted
                        break;
                    continue;
                case 0:
                    sysManager->performCompleteLogout(currentUserID);
                    isLoggedIn = false;
                    currentUserID = 0;
                    printSuccess("Logged out successfully!");
                    pause();
                    break;
                default:
                    printError("Invalid choice!");
                    pause();
                    break;
                }
            }
        }
    }
};

// ============================================================================
// MAIN FUNCTION
// ============================================================================

int main()
{
    try
    {
        SocialPlatformCLI app;
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << RED << "Fatal error: " << e.what() << RESET << "\n";
        return 1;
    }
    catch (...)
    {
        std::cerr << RED << "Unknown fatal error occurred!" << RESET << "\n";
        return 1;
    }

    return 0;
}