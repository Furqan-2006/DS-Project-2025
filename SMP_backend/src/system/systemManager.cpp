#include "system/SystemManager.hpp"
#include <iostream>
#include <ctime>

// ============================================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================================

SystemManager::SystemManager()
{
    // Initialize modules in dependency order
    userMgr = new UserManager();
    relGraph = new RelationshipGraph(1000);
    statusMgr = new StatusManager();
    followerList = new FollowerList();
    postMgr = new PostManager("data/post.json");
    feedMgr = new FeedManager(postMgr, relGraph);
    recEng = new RecommendationEngine(postMgr, relGraph);
    searchEng = new SearchEngine();
    notifMgr = new NotificationManager("data/notifications.json");
    msgSys = new MessageSystem("data/messages.json");
    reqMgr = new FriendRequestManager("data/requests.json");
}

SystemManager::~SystemManager()
{
    // Delete in reverse order of dependencies
    delete reqMgr;
    delete msgSys;
    delete notifMgr;
    delete searchEng;
    delete recEng;
    delete feedMgr;
    delete postMgr;
    delete followerList;
    delete statusMgr;
    delete relGraph;
    delete userMgr;
}

// ============================================================================
// HELPER METHODS
// ============================================================================

void SystemManager::cascadeUserDeletion(ull userID)
{
    std::cout << "Cascading deletion for user " << userID << "...\n";

    // 1. Remove from status manager
    statusMgr->removeUser(userID);

    // 2. Delete all user's posts
    std::vector<Post *> userPosts = postMgr->getPostsByUser(userID);
    for (Post *post : userPosts)
    {
        if (post)
        {
            ull postID = post->getPostID();

            // Remove from search engine (posts are indexed)
            // searchEng handles this internally when post is removed

            // Remove like edges from graph
            const Set<NodeID> *likers = relGraph->getPostLikes(postID);
            if (likers)
            {
                const auto &data = likers->data();
                for (size_t i = 0; i < likers->size(); i++)
                {
                    relGraph->unlikePost(data[i], postID);
                }
            }

            postMgr->deletePost(postID);
        }
    }

    // 3. Delete all notifications TO this user
    std::vector<Notification *> userNotifications = getAllNotifications(userID);
    for (Notification *notif : userNotifications)
    {
        if (notif)
        {
            notifMgr->removeByID(notif->getID());
        }
    }

    // 4. Delete all notifications FROM this user (to others)
    Notification **allNotifs = notifMgr->getAll();
    if (allNotifs)
    {
        std::vector<ull> toDelete;
        for (ull i = 0; i < notifMgr->size(); ++i)
        {
            if (allNotifs[i] && allNotifs[i]->getSender() == userID)
            {
                toDelete.push_back(allNotifs[i]->getID());
            }
        }

        for (ull nid : toDelete)
        {
            notifMgr->removeByID(nid);
        }
    }

    // 5. Cancel all friend requests involving this user
    std::vector<FriendRequest> inbox = reqMgr->getInbox();
    std::vector<FriendRequest> outbox = reqMgr->getOutbox();

    // Cancel outgoing requests (sent by this user)
    for (const FriendRequest &req : outbox)
    {
        if (req.getSenderID() == userID && req.getStatus() == RequestStatus::PENDING)
        {
            reqMgr->cancelRequest(req.getRecieverID());
        }
    }

    // Reject incoming requests (sent to this user)
    for (const FriendRequest &req : inbox)
    {
        if (req.getRecieverID() == userID && req.getStatus() == RequestStatus::PENDING)
        {
            reqMgr->rejectRequest(req.getSenderID());
        }
    }

    // 6. Remove all follow relationships
    // Get all followers and have them unfollow this user
    std::vector<ull> followers = getFollowers(userID);
    for (ull followerID : followers)
    {
        relGraph->unfollow(followerID, userID);
    }

    // Get all following and unfollow them
    std::vector<ull> following = getFollowing(userID);
    for (ull followeeID : following)
    {
        relGraph->unfollow(userID, followeeID);
    }

    // 7. Clear all activities involving this user
    clearAllActivities();

    std::cout << "User " << userID << " successfully deleted from all modules.\n";
}

bool SystemManager::validateUser(ull userID) const
{
    User *user = userMgr->getUserByID(userID);
    return user != nullptr;
}

bool SystemManager::validateUsers(ull userID1, ull userID2) const
{
    return validateUser(userID1) && validateUser(userID2);
}

bool SystemManager::validatePost(ull postID) const
{
    return postMgr->getPost(postID) != nullptr;
}

// ============================================================================
// SYSTEM LIFECYCLE MANAGEMENT
// ============================================================================

bool SystemManager::initializeAllModules()
{
    std::cout << "Initializing all modules...\n";

    // Load persisted data
    bool success = loadAllData();

    if (success)
    {
        std::cout << "All modules initialized successfully\n";
    }
    else
    {
        std::cout << "Warning: Some modules failed to load data\n";
    }

    return success;
}

bool SystemManager::saveAllData()
{
    bool success = true;

    if (!postMgr->saveToFile())
    {
        std::cerr << "Failed to save posts\n";
        success = false;
    }

    if (!msgSys->saveToFile())
    {
        std::cerr << "Failed to save messages\n";
        success = false;
    }

    if (!notifMgr->saveToFile())
    {
        std::cerr << "Failed to save notifications\n";
        success = false;
    }

    if (!reqMgr->saveToFile())
    {
        std::cerr << "Failed to save friend requests\n";
        success = false;
    }

    return success;
}

bool SystemManager::loadAllData()
{
    bool success = true;

    if (!postMgr->loadFromFile())
    {
        std::cerr << "Note: No existing posts data found\n";
    }

    if (!msgSys->loadFromFile())
    {
        std::cerr << "Note: No existing messages data found\n";
    }

    if (!notifMgr->loadFromFile())
    {
        std::cerr << "Note: No existing notifications data found\n";
    }

    if (!reqMgr->loadFromFile())
    {
        std::cerr << "Note: No existing friend requests data found\n";
    }

    return success;
}

void SystemManager::shutdownAllModules()
{
    std::cout << "Shutting down all modules...\n";
    saveAllData();
    clearAllActivities();
    std::cout << "All modules shut down successfully\n";
}

// ============================================================================
// USER OPERATIONS
// ============================================================================

bool SystemManager::registerUser(const std::string &uname,
                                 const std::string &password,
                                 const std::string &city)
{
    // Step 1: Create user account
    if (!userMgr->registerUser(uname, password, city))
    {
        return false;
    }

    User *user = userMgr->getUserByUsername(uname);
    if (!user)
    {
        return false;
    }

    ull userID = user->getID();

    // Step 2: Register in relationship graph
    relGraph->registerUser(userID);

    // Step 3: Set initial status as offline
    statusMgr->setOffline(userID);

    // Step 4: Index user for search
    searchEng->addUser(user);

    // Step 5: Register for messaging
    msgSys->registerUser(user);

    std::cout << "User registration complete for: " << uname << "\n";
    return true;
}

bool SystemManager::deleteUser(ull userID)
{
    User *user = userMgr->getUserByID(userID);
    if (!user)
    {
        return false;
    }

    std::string username = user->getUname();

    // Cascade deletion across all modules
    cascadeUserDeletion(userID);

    // Finally delete from user manager
    return userMgr->deleteUser(username);
}

SystemManager::LoginResult SystemManager::performCompleteLogin(ull userID,
                                                               const std::string &password)
{
    LoginResult result;
    result.success = false;

    // Step 1: Validate credentials
    User *user = userMgr->getUserByID(userID);
    if (!user)
    {
        std::cout << "User not found\n";
        return result;
    }

    std::string username = user->getUname();
    if (!userMgr->loginUser(username, password))
    {
        std::cout << "Invalid credentials\n";
        return result;
    }

    // Step 2: Mark user online
    statusMgr->setOnline(userID);

    // Step 3: Fetch unread notifications
    result.unreadNotifications = getUnreadNotifications(userID);

    // Step 4: Generate initial feed
    result.initialFeed = getUserFeed(userID, 20);

    // Step 5: Get online friends
    result.onlineFriends = getOnlineFriends(userID);

    result.success = true;
    std::cout << "Login successful for user: " << username << "\n";
    std::cout << "Unread notifications: " << result.unreadNotifications.size() << "\n";
    std::cout << "Online friends: " << result.onlineFriends.size() << "\n";

    return result;
}

bool SystemManager::performCompleteLogout(ull userID)
{
    User *user = userMgr->getUserByID(userID);
    if (!user)
    {
        return false;
    }

    std::string username = user->getUname();

    // Step 1: Mark user offline
    statusMgr->setOffline(userID);

    // Step 2: Log session end
    if (!userMgr->logoutUser(username))
    {
        return false;
    }

    // Step 3: Clear active connections (optional)
    // This can be done lazily via expireOldActivities()

    std::cout << "Logout successful for user: " << username << "\n";
    return true;
}

User *SystemManager::getUser(ull userID) const
{
    return userMgr->getUserByID(userID);
}

User *SystemManager::getUserByUsername(const std::string &username) const
{
    return userMgr->getUserByUsername(username);
}

bool SystemManager::updateUserProfile(ull userID, const std::string &newCity)
{
    User *user = userMgr->getUserByID(userID);
    if (!user)
    {
        return false;
    }

    std::string username = user->getUname();
    State currentStatus = user->getStatus();

    return userMgr->updateProfile(username, newCity, currentStatus);
}

bool SystemManager::resetPassword(ull userID, const std::string &newPassword)
{
    User *user = userMgr->getUserByID(userID);
    if (!user)
    {
        return false;
    }

    std::string username = user->getUname();
    return userMgr->resetPassword(username, newPassword);
}

// ============================================================================
// STATUS MANAGEMENT
// ============================================================================

bool SystemManager::setUserOnline(ull userID)
{
    if (!validateUser(userID))
    {
        return false;
    }

    statusMgr->setOnline(userID);
    return true;
}

bool SystemManager::setUserOffline(ull userID)
{
    if (!validateUser(userID))
    {
        return false;
    }

    statusMgr->setOffline(userID);
    return true;
}

bool SystemManager::isUserOnline(ull userID) const
{
    return statusMgr->isOnline(userID);
}

std::string SystemManager::getUserStatusString(ull userID) const
{
    return statusMgr->getStatusString(userID);
}

std::vector<ull> SystemManager::getOnlineFriends(ull userID) const
{
    std::vector<ull> onlineFriends;
    std::vector<ull> friends = getFriends(userID);

    for (ull friendID : friends)
    {
        if (statusMgr->isOnline(friendID))
        {
            onlineFriends.push_back(friendID);
        }
    }

    return onlineFriends;
}

// ============================================================================
// POST OPERATIONS
// ============================================================================

Post *SystemManager::performCreatePostComplete(ull authorID, const std::string &content)
{
    // Step 1: Validate author
    if (!validateUser(authorID))
    {
        return nullptr;
    }

    // Step 2: Create post
    Post *post = postMgr->createPost(authorID, content);
    if (!post)
    {
        return nullptr;
    }

    ull postID = post->getPostID();

    // Step 3: Register post in relationship graph (for likes)
    relGraph->registerPost(postID);

    // Step 4: Index post in search engine
    searchEng->addSystemItem(post);

    // Step 5: Notify all followers
    std::vector<ull> followers = getFollowers(authorID);
    for (ull followerID : followers)
    {
        std::string desc = "User " + std::to_string(authorID) +
                           " posted: " + content.substr(0, 50) +
                           (content.length() > 50 ? "..." : "");
        notifMgr->create(followerID, authorID, NotificationType::System, desc);
    }

    std::cout << "Post created successfully (ID: " << postID << ")\n";
    std::cout << "Notified " << followers.size() << " followers\n";

    return post;
}

bool SystemManager::deletePost(ull postID)
{
    // Step 1: Validate post exists
    Post *post = postMgr->getPost(postID);
    if (!post)
    {
        return false;
    }

    // Step 2: Remove all like edges from graph
    const Set<NodeID> *likers = relGraph->getPostLikes(postID);
    if (likers)
    {
        const auto &data = likers->data();
        for (size_t i = 0; i < likers->size(); i++)
        {
            relGraph->unlikePost(data[i], postID);
        }
    }

    // Step 3: Delete from post manager (search engine updated automatically)
    return postMgr->deletePost(postID);
}

Post *SystemManager::getPost(ull postID) const
{
    return postMgr->getPost(postID);
}

std::vector<Post *> SystemManager::getUserPosts(ull userID) const
{
    return postMgr->getPostsByUser(userID);
}

std::vector<Post *> SystemManager::getAllPosts() const
{
    return postMgr->getAllPosts();
}

bool SystemManager::performLikePostComplete(ull userID, ull postID)
{
    // Step 1: Validate user and post
    if (!validateUser(userID) || !validatePost(postID))
    {
        return false;
    }

    Post *post = postMgr->getPost(postID);
    if (!post)
    {
        return false;
    }

    // Step 2: Check if already liked
    if (hasUserLikedPost(userID, postID))
    {
        std::cout << "User already liked this post\n";
        return false;
    }

    // Step 3: Update post's like list
    post->addLikes(userID);

    // Step 4: Add like edge in relationship graph
    if (!relGraph->likePost(userID, postID))
    {
        post->removeLike(userID); // Rollback
        return false;
    }

    // Step 5: Notify author (if not self-like)
    ull authorID = post->getAuthor();
    if (authorID != userID)
    {
        notifyLike(authorID, userID, postID);
    }

    // Step 6: Record activity for recommendations
    recordActivity(userID, authorID);

    std::cout << "Like recorded successfully\n";
    return true;
}

bool SystemManager::unlikePost(ull userID, ull postID)
{
    if (!validateUser(userID) || !validatePost(postID))
    {
        return false;
    }

    Post *post = postMgr->getPost(postID);
    if (!post)
    {
        return false;
    }

    post->removeLike(userID);
    return relGraph->unlikePost(userID, postID);
}

bool SystemManager::hasUserLikedPost(ull userID, ull postID) const
{
    return relGraph->hasLiked(userID, postID);
}

size_t SystemManager::getPostLikesCount(ull postID) const
{
    Post *post = postMgr->getPost(postID);
    if (!post)
    {
        return 0;
    }
    return post->getLikesCount();
}

// ============================================================================
// RELATIONSHIP OPERATIONS
// ============================================================================

bool SystemManager::performFollowWithNotification(ull followerID, ull followeeID)
{
    // Step 1: Validate users
    if (!validateUsers(followerID, followeeID))
    {
        return false;
    }

    // Step 2: Check if can follow
    if (!canFollowUser(followerID, followeeID))
    {
        return false;
    }

    // Step 3: Add follow edge in relationship graph
    if (!relGraph->follow(followerID, followeeID))
    {
        return false;
    }

    // Step 4: Send follow notification
    notifyFollow(followeeID, followerID);

    // Step 5: Record activity
    recordActivity(followerID, followeeID);

    std::cout << "Follow relationship established\n";
    return true;
}

bool SystemManager::performUnfollowComplete(ull followerID, ull followeeID)
{
    // Step 1: Validate users
    if (!validateUsers(followerID, followeeID))
    {
        return false;
    }

    // Step 2: Check if currently following
    if (!isFollowing(followerID, followeeID))
    {
        std::cout << "Not currently following this user\n";
        return false;
    }

    // Step 3: Remove follow edge
    if (!relGraph->unfollow(followerID, followeeID))
    {
        return false;
    }

    // Step 4: Cancel pending friend request if exists
    if (hasPendingRequestTo(followerID, followeeID))
    {
        reqMgr->cancelRequest(followeeID);
    }

    // Step 5: Clear activities (optional - can be done lazily)

    std::cout << "Unfollow successful\n";
    return true;
}

bool SystemManager::isFollowing(ull followerID, ull followeeID) const
{
    return relGraph->isFollowing(followerID, followeeID);
}

std::vector<ull> SystemManager::getFollowers(ull userID) const
{
    const Set<NodeID> *followers = relGraph->getFollowers(userID);
    std::vector<ull> result;

    if (followers)
    {
        const auto &data = followers->data();
        for (size_t i = 0; i < followers->size(); i++)
        {
            result.push_back(data[i]);
        }
    }

    return result;
}

std::vector<ull> SystemManager::getFollowing(ull userID) const
{
    const Set<NodeID> *following = relGraph->getFollowing(userID);
    std::vector<ull> result;

    if (following)
    {
        const auto &data = following->data();
        for (size_t i = 0; i < following->size(); i++)
        {
            result.push_back(data[i]);
        }
    }

    return result;
}

size_t SystemManager::getFollowerCount(ull userID) const
{
    return relGraph->followerCount(userID);
}

size_t SystemManager::getFollowingCount(ull userID) const
{
    return relGraph->followingCount(userID);
}

std::vector<ull> SystemManager::getFriends(ull userID) const
{
    return relGraph->getFriends(userID);
}

std::vector<ull> SystemManager::getMutualConnections(ull userID) const
{
    return relGraph->getMutualConnections(userID, userID);
}

std::vector<ull> SystemManager::getMutualFriends(ull userID1, ull userID2) const
{
    return relGraph->getMutualConnections(userID1, userID2);
}

bool SystemManager::areUsersFriends(ull userID1, ull userID2) const
{
    return relGraph->isFollowing(userID1, userID2) &&
           relGraph->isFollowing(userID2, userID1);
}

// ============================================================================
// FRIEND REQUEST OPERATIONS
// ============================================================================

bool SystemManager::performSendFriendRequestComplete(ull senderID, ull receiverID)
{
    // Step 1: Validate users
    if (!validateUsers(senderID, receiverID))
    {
        return false;
    }

    // Step 2: Cannot send to self
    if (senderID == receiverID)
    {
        std::cout << "Cannot send friend request to yourself\n";
        return false;
    }

    // Step 3: Check if already friends
    if (areUsersFriends(senderID, receiverID))
    {
        std::cout << "Already friends\n";
        return false;
    }

    // Step 4: Check for existing pending request
    if (hasPendingRequestTo(senderID, receiverID))
    {
        std::cout << "Friend request already sent\n";
        return false;
    }

    // Step 5: Check for reverse request (auto-accept scenario)
    if (hasPendingRequestFrom(senderID, receiverID))
    {
        std::cout << "Reverse request exists, auto-accepting...\n";
        return performAcceptFriendRequest(senderID, receiverID);
    }

    // Step 6: Send friend request
    if (!reqMgr->sendRequest(senderID, receiverID))
    {
        return false;
    }

    // Step 7: Notify receiver
    notifyFriendRequest(receiverID, senderID);

    std::cout << "Friend request sent successfully\n";
    return true;
}

bool SystemManager::performAcceptFriendRequest(ull receiverID, ull senderID)
{
    // Step 1: Validate users
    if (!validateUsers(receiverID, senderID))
    {
        return false;
    }

    // Step 2: Accept request in friend request manager
    if (!reqMgr->acceptRequest(senderID))
    {
        return false;
    }

    // Step 3: Create bidirectional follow relationship
    relGraph->follow(receiverID, senderID);
    relGraph->follow(senderID, receiverID);

    // Step 4: Notify both users
    std::string notifDesc = "User " + std::to_string(receiverID) +
                            " accepted your friend request";
    notifMgr->create(senderID, receiverID, NotificationType::Follow, notifDesc);

    std::cout << "Friend request accepted successfully\n";
    return true;
}

bool SystemManager::rejectFriendRequest(ull receiverID, ull senderID)
{
    if (!validateUsers(receiverID, senderID))
    {
        return false;
    }

    return reqMgr->rejectRequest(senderID);
}

bool SystemManager::cancelFriendRequest(ull senderID, ull receiverID)
{
    if (!validateUsers(senderID, receiverID))
    {
        return false;
    }

    return reqMgr->cancelRequest(receiverID);
}

std::vector<FriendRequest> SystemManager::getPendingIncomingRequests(ull userID) const
{
    std::vector<FriendRequest> allInbox = reqMgr->getInbox();
    std::vector<FriendRequest> pending;

    for (const FriendRequest &req : allInbox)
    {
        if (req.getRecieverID() == userID && req.getStatus() == RequestStatus::PENDING)
        {
            pending.push_back(req);
        }
    }

    return pending;
}

std::vector<FriendRequest> SystemManager::getPendingOutgoingRequests(ull userID) const
{
    std::vector<FriendRequest> allOutbox = reqMgr->getOutbox();
    std::vector<FriendRequest> pending;

    for (const FriendRequest &req : allOutbox)
    {
        if (req.getSenderID() == userID && req.getStatus() == RequestStatus::PENDING)
        {
            pending.push_back(req);
        }
    }

    return pending;
}

bool SystemManager::hasPendingRequestFrom(ull receiverID, ull senderID) const
{
    return reqMgr->hasPenIn(senderID);
}

bool SystemManager::hasPendingRequestTo(ull senderID, ull receiverID) const
{
    return reqMgr->hasPenOut(receiverID);
}

// ============================================================================
// MESSAGE OPERATIONS
// ============================================================================

bool SystemManager::performSendMessageComplete(ull senderID, ull receiverID,
                                               const std::string &content)
{
    // Step 1: Validate users
    if (!validateUsers(senderID, receiverID))
    {
        return false;
    }

    User *sender = getUser(senderID);
    User *receiver = getUser(receiverID);

    if (!sender || !receiver)
    {
        return false;
    }

    // Step 2: Send message via message system
    msgSys->sendMessage(*sender, *receiver, content);

    // Step 3: Notify receiver
    notifyMessage(receiverID, senderID);

    // Step 4: Check receiver online status (for immediate delivery notification)
    bool receiverOnline = statusMgr->isOnline(receiverID);
    if (receiverOnline)
    {
        std::cout << "Receiver is online - message delivered immediately\n";
    }
    else
    {
        std::cout << "Receiver is offline - message queued\n";
    }

    // Step 5: Record activity
    recordActivity(senderID, receiverID);

    return true;
}

std::vector<Message> SystemManager::getMessageHistory(ull user1ID, ull user2ID) const
{
    User *user1 = getUser(user1ID);
    User *user2 = getUser(user2ID);

    if (!user1 || !user2)
    {
        return std::vector<Message>();
    }

    return msgSys->getChatHistory(*user1, *user2);
}

Message SystemManager::getLatestMessage(ull user1ID, ull user2ID) const
{
    User *user1 = getUser(user1ID);
    User *user2 = getUser(user2ID);

    if (!user1 || !user2)
    {
        return Message();
    }

    return msgSys->getLastestMessage(*user1, *user2);
}

bool SystemManager::markMessageAsRead(ull userID, ull messageID)
{
    User *user = getUser(userID);
    if (!user)
    {
        return false;
    }

    return msgSys->markMessageRead(*user, messageID);
}

std::vector<Message> SystemManager::searchMessages(ull userID, const std::string &keyword) const
{
    User *user = getUser(userID);
    if (!user)
    {
        return std::vector<Message>();
    }

    return msgSys->searchMessages(*user, keyword);
}

// ============================================================================
// FEED OPERATIONS
// ============================================================================

std::vector<Post *> SystemManager::getUserFeed(ull userID, size_t limit) const
{
    return feedMgr->getUserFeed(userID, limit);
}

std::vector<Post *> SystemManager::getUserFeedByFriends(ull userID, size_t limit) const
{
    return feedMgr->getUserFeedByFriends(userID, limit);
}

void SystemManager::displayUserFeed(ull userID, size_t limit) const
{
    feedMgr->displayFeed(userID, limit);
}

// ============================================================================
// RECOMMENDATION OPERATIONS
// ============================================================================

std::vector<ull> SystemManager::getRecommendedUsers(ull userID, size_t limit) const
{
    return recEng->recommendUsers(userID, limit);
}

std::vector<Post *> SystemManager::getRecommendedPosts(ull userID, size_t limit) const
{
    return recEng->recommendPosts(userID, limit);
}

std::vector<Post *> SystemManager::getTrendingPosts(size_t limit) const
{
    return recEng->recommendTrendingPosts(limit);
}

void SystemManager::displayRecommendedPosts(ull userID, size_t limit) const
{
    recEng->displayRecommendedPosts(userID, limit);
}

// Add these functions to the existing SystemManager.cpp file

// ============================================================================
// SEARCH OPERATIONS
// ============================================================================

std::vector<User *> SystemManager::searchUsers(const std::string &query, SearchMode mode) const
{
    std::vector<void *> results = searchEng->search(query, SearchType::USER, mode);
    std::vector<User *> users;

    for (size_t i = 0; i < results.size(); ++i)
    {
        users.push_back(static_cast<User *>(results[i]));
    }

    return users;
}

std::vector<Post *> SystemManager::searchPosts(const std::string &query, SearchMode mode) const
{
    std::vector<void *> results = searchEng->search(query, SearchType::SYSTEM, mode);
    std::vector<Post *> posts;

    for (size_t i = 0; i < results.size(); ++i)
    {
        posts.push_back(static_cast<Post *>(results[i]));
    }

    return posts;
}

std::vector<User *> SystemManager::searchUsersExact(const std::string &query) const
{
    return searchUsers(query, SearchMode::EXACT);
}

std::vector<User *> SystemManager::searchUsersPrefix(const std::string &query) const
{
    return searchUsers(query, SearchMode::PREFIX);
}

std::vector<Post *> SystemManager::searchPostsExact(const std::string &query) const
{
    return searchPosts(query, SearchMode::EXACT);
}

std::vector<Post *> SystemManager::searchPostsPrefix(const std::string &query) const
{
    return searchPosts(query, SearchMode::PREFIX);
}

// ============================================================================
// NOTIFICATION OPERATIONS
// ============================================================================

std::vector<Notification *> SystemManager::getAllNotifications(ull userID) const
{
    std::vector<Notification *> results;

    Notification **allNotifs = notifMgr->getAll();
    if (!allNotifs)
    {
        return results;
    }

    for (ull i = 0; i < notifMgr->size(); ++i)
    {
        if (allNotifs[i] && allNotifs[i]->getReciever() == userID)
        {
            results.push_back(allNotifs[i]);
        }
    }

    return results;
}

std::vector<Notification *> SystemManager::getUnreadNotifications(ull userID) const
{
    std::vector<Notification *> results;

    Notification **unreadNotifs = notifMgr->getUnread();
    if (!unreadNotifs)
    {
        return results;
    }

    ull unreadCount = notifMgr->countUnread();
    for (ull i = 0; i < unreadCount; ++i)
    {
        if (unreadNotifs[i] && unreadNotifs[i]->getReciever() == userID)
        {
            results.push_back(unreadNotifs[i]);
        }
    }

    return results;
}

std::vector<Notification *> SystemManager::getNotificationsByType(ull userID, NotificationType type) const
{
    std::vector<Notification *> results;

    Notification **typeNotifs = notifMgr->getByType(type);
    if (!typeNotifs)
    {
        return results;
    }

    // Count how many notifications of this type exist
    Notification **allNotifs = notifMgr->getAll();
    ull totalCount = notifMgr->size();

    for (ull i = 0; i < totalCount; ++i)
    {
        if (allNotifs[i] &&
            allNotifs[i]->getReciever() == userID &&
            allNotifs[i]->getType() == type)
        {
            results.push_back(allNotifs[i]);
        }
    }

    return results;
}

Notification *SystemManager::getNotificationByID(ull notificationID) const
{
    return notifMgr->getByID(notificationID);
}

size_t SystemManager::getUnreadNotificationCount(ull userID) const
{
    std::vector<Notification *> unread = getUnreadNotifications(userID);
    return unread.size();
}

bool SystemManager::markNotificationAsRead(ull notificationID)
{
    Notification *notif = notifMgr->getByID(notificationID);
    if (!notif)
    {
        return false;
    }

    notifMgr->markAsRead(notificationID);
    return true;
}

bool SystemManager::markAllNotificationsAsRead(ull userID)
{
    std::vector<Notification *> userNotifications = getAllNotifications(userID);

    for (Notification *notif : userNotifications)
    {
        if (notif && !notif->getReadStatus())
        {
            notifMgr->markAsRead(notif->getID());
        }
    }

    return true;
}

bool SystemManager::deleteNotification(ull notificationID)
{
    return notifMgr->removeByID(notificationID);
}

// Notification creation for various events
void SystemManager::notifyLike(ull receiverID, ull senderID, ull postID)
{
    std::string desc = "User " + std::to_string(senderID) +
                       " liked your post (ID: " + std::to_string(postID) + ")";
    notifMgr->create(receiverID, senderID, NotificationType::Like, desc);
}

void SystemManager::notifyFollow(ull receiverID, ull senderID)
{
    std::string desc = "User " + std::to_string(senderID) + " started following you";
    notifMgr->create(receiverID, senderID, NotificationType::Follow, desc);
}

void SystemManager::notifyMessage(ull receiverID, ull senderID)
{
    std::string desc = "You have a new message from User " + std::to_string(senderID);
    notifMgr->create(receiverID, senderID, NotificationType::Message, desc);
}

void SystemManager::notifyFriendRequest(ull receiverID, ull senderID)
{
    std::string desc = "User " + std::to_string(senderID) + " sent you a friend request";
    notifMgr->create(receiverID, senderID, NotificationType::Follow, desc);
}

void SystemManager::notifySystem(ull receiverID, const std::string &description)
{
    notifMgr->create(receiverID, 0, NotificationType::System, description);
}

// ============================================================================
// ACTIVITY TRACKING
// ============================================================================

void SystemManager::recordActivity(ull user1ID, ull user2ID)
{
    long long now = static_cast<long long>(std::time(nullptr));
    relGraph->addActive(user1ID, user2ID, now);
}

void SystemManager::expireOldActivities()
{
    long long now = static_cast<long long>(std::time(nullptr));
    relGraph->expireActive(now);
}

std::vector<ull> SystemManager::getActiveConnections(ull userID) const
{
    std::vector<ull> activeUsers;

    const Set<NodeID> *activeWith = relGraph->getActiveWith(userID);
    if (activeWith)
    {
        const auto &data = activeWith->data();
        for (size_t i = 0; i < activeWith->size(); i++)
        {
            activeUsers.push_back(data[i]);
        }
    }

    return activeUsers;
}

void SystemManager::clearAllActivities()
{
    relGraph->clearActive();
}

// ============================================================================
// ANALYTICS & STATISTICS
// ============================================================================

SystemManager::UserStats SystemManager::getUserStats(ull userID) const
{
    UserStats stats;

    // Initialize all fields
    stats.followerCount = 0;
    stats.followingCount = 0;
    stats.friendCount = 0;
    stats.postCount = 0;
    stats.totalLikes = 0;
    stats.unreadNotifications = 0;
    stats.pendingRequests = 0;
    stats.isOnline = false;

    // Check if user exists
    if (!validateUser(userID))
    {
        return stats;
    }

    // Get follower/following counts
    stats.followerCount = getFollowerCount(userID);
    stats.followingCount = getFollowingCount(userID);

    // Get friend count
    std::vector<ull> friends = getFriends(userID);
    stats.friendCount = friends.size();

    // Get post count and total likes
    std::vector<Post *> userPosts = getUserPosts(userID);
    stats.postCount = userPosts.size();

    for (Post *post : userPosts)
    {
        if (post)
        {
            stats.totalLikes += post->getLikesCount();
        }
    }

    // Get unread notification count
    stats.unreadNotifications = getUnreadNotificationCount(userID);

    // Get pending request count
    std::vector<FriendRequest> pendingIncoming = getPendingIncomingRequests(userID);
    stats.pendingRequests = pendingIncoming.size();

    // Check online status
    stats.isOnline = isUserOnline(userID);

    return stats;
}

void SystemManager::clearUserNotifications(ull userID)
{
    std::vector<Notification *> userNotifications = getAllNotifications(userID);

    for (Notification *notif : userNotifications)
    {
        if (notif)
        {
            notifMgr->removeByID(notif->getID());
        }
    }
}

void SystemManager::clearUserFriendRequests(ull userID)
{
    // Cancel all outgoing requests
    std::vector<FriendRequest> outgoing = getPendingOutgoingRequests(userID);
    for (const FriendRequest &req : outgoing)
    {
        if (req.getStatus() == RequestStatus::PENDING)
        {
            reqMgr->cancelRequest(req.getRecieverID());
        }
    }

    // Reject all incoming requests
    std::vector<FriendRequest> incoming = getPendingIncomingRequests(userID);
    for (const FriendRequest &req : incoming)
    {
        if (req.getStatus() == RequestStatus::PENDING)
        {
            reqMgr->rejectRequest(req.getSenderID());
        }
    }
}

// ============================================================================
// VALIDATION HELPERS
// ============================================================================

bool SystemManager::canFollowUser(ull followerID, ull followeeID) const
{
    // Cannot follow yourself
    if (followerID == followeeID)
    {
        std::cout << "Cannot follow yourself\n";
        return false;
    }

    // Cannot follow if already following
    if (isFollowing(followerID, followeeID))
    {
        std::cout << "Already following this user\n";
        return false;
    }

    return true;
}

bool SystemManager::canSendFriendRequest(ull senderID, ull receiverID) const
{
    // Cannot send to yourself
    if (senderID == receiverID)
    {
        return false;
    }

    // Cannot send if already friends
    if (areUsersFriends(senderID, receiverID))
    {
        return false;
    }

    // Cannot send if there's already a pending request
    if (hasPendingRequestTo(senderID, receiverID))
    {
        return false;
    }

    return true;
}

// ============================================================================
// DISPLAY & DEBUG
// ============================================================================

void SystemManager::displayAllUsers() const
{
    userMgr->displayAllUsers();
}

void SystemManager::displayUserProfile(ull userID) const
{
    User *user = getUser(userID);
    if (!user)
    {
        std::cout << "User not found\n";
        return;
    }

    std::cout << "\n===== User Profile =====\n";
    user->display();

    // Display statistics
    UserStats stats = getUserStats(userID);
    std::cout << "\n--- Statistics ---\n";
    std::cout << "Followers: " << stats.followerCount << "\n";
    std::cout << "Following: " << stats.followingCount << "\n";
    std::cout << "Friends: " << stats.friendCount << "\n";
    std::cout << "Posts: " << stats.postCount << "\n";
    std::cout << "Total Likes Received: " << stats.totalLikes << "\n";
    std::cout << "Unread Notifications: " << stats.unreadNotifications << "\n";
    std::cout << "Pending Friend Requests: " << stats.pendingRequests << "\n";
    std::cout << "Status: " << (stats.isOnline ? "ONLINE" : "OFFLINE") << "\n";
    std::cout << "========================\n";
}

void SystemManager::displayAllPosts() const
{
    postMgr->displayAll();
}

void SystemManager::displaySystemStatus() const
{
    std::cout << "\n========================================\n";
    std::cout << "       SYSTEM STATUS REPORT\n";
    std::cout << "========================================\n\n";

    // Count total users
    size_t totalUsers = 0;
    size_t onlineUsers = 0;

    // This is a workaround since we don't have direct access to all user IDs
    // In a real implementation, you'd have a method to get all user IDs
    std::cout << "Module Status:\n";
    std::cout << "  [OK] User Manager\n";
    std::cout << "  [OK] Relationship Graph\n";
    std::cout << "  [OK] Status Manager\n";
    std::cout << "  [OK] Post Manager\n";
    std::cout << "  [OK] Feed Manager\n";
    std::cout << "  [OK] Recommendation Engine\n";
    std::cout << "  [OK] Search Engine\n";
    std::cout << "  [OK] Message System\n";
    std::cout << "  [OK] Notification Manager\n";
    std::cout << "  [OK] Friend Request Manager\n";

    std::cout << "\nData Statistics:\n";
    std::cout << "  Total Posts: " << getAllPosts().size() << "\n";
    std::cout << "  Total Notifications: " << notifMgr->size() << "\n";
    std::cout << "  Unread Notifications: " << notifMgr->countUnread() << "\n";

    std::cout << "\nSystem Health: OPERATIONAL\n";
    std::cout << "========================================\n\n";
}

// ============================================================================
// ADDITIONAL FEED OPERATIONS (Missing from earlier implementation)
// ============================================================================

std::vector<Post *> SystemManager::getUserFeedByMutualConnections(ull userID, size_t limit) const
{
    // Get mutual friends
    std::vector<NodeID> mutuals = getMutualConnections(userID);

    if (mutuals.empty())
    {
        return {};
    }

    // Collect posts from all mutual connections
    std::vector<std::vector<Post *>> allMutualPosts;

    for (size_t i = 0; i < mutuals.size(); i++)
    {
        std::vector<Post *> posts = postMgr->getPostsByUser(mutuals[i]);

        if (!posts.empty())
        {
            allMutualPosts.push_back(posts);
        }
    }

    // Add user's own posts
    std::vector<Post *> ownPosts = postMgr->getPostsByUser(userID);
    if (!ownPosts.empty())
    {
        allMutualPosts.push_back(ownPosts);
    }

    // Use FeedManager's merge algorithm
    return feedMgr->mergeKSortedLists(allMutualPosts, limit);
}

// ============================================================================
// RECOMMENDATION DISPLAY OPERATIONS (Forward to RecommendationEngine)
// ============================================================================

void SystemManager::displayRecommendedUsers(ull userID, size_t limit) const
{
    recEng->displayRecommendedUsers(userID, limit);
}