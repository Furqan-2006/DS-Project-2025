#pragma once

#include "core/followerList.hpp"
#include "core/relationGraph.hpp"
#include "core/status.hpp"
#include "core/user.hpp"

#include "content/feed.hpp"
#include "content/post.hpp"
#include "content/recommendation.hpp"

#include "interaction/friendRequest.hpp"
#include "interaction/message.hpp"

#include "system/search.hpp"
#include "system/notification.hpp"

/**
 * SYSTEM MANAGER - Central Orchestration Layer
 *
 * Acts as the "operating system" of Mini-Instagram backend.
 * Responsibilities:
 * 1. Module Lifecycle Management
 * 2. Workflow Orchestration (multi-step operations)
 * 3. Data Routing & Integrity Control
 * 4. File I/O Coordination
 * 5. Performance Coordination
 *
 * Design Principles:
 * - Loose coupling between modules
 * - High cohesion within workflows
 * - Centralized control flow
 * - Robust data integrity
 * - Predictable inter-module behavior
 */

class SystemManager
{
private:
    // =======================
    // Core Module Instances
    // =======================
    UserManager *userMgr;        // User authentication & profiles
    RelationshipGraph *relGraph; // Follow/Friend graph + likes
    StatusManager *statusMgr;    // Online/Offline tracking
    FollowerList *followerList;  // Efficient follower queries

    // Content Management
    PostManager *postMgr;         // Post CRUD operations
    FeedManager *feedMgr;         // Feed generation algorithms
    RecommendationEngine *recEng; // User/Post recommendations
    SearchEngine *searchEng;      // Search with multiple algorithms

    // Interaction Management
    MessageSystem *msgSys;         // Message queues & history
    NotificationManager *notifMgr; // Notification system
    FriendRequestManager *reqMgr;  // Friend request workflows

    // =======================
    // Helper Methods
    // =======================

    /**
     * Cascading deletion across all modules
     * Ensures data integrity when removing users
     */
    void cascadeUserDeletion(ull userID);

    /**
     * Validation helpers to prevent invalid operations
     */
public:
    bool validateUser(ull userID) const;
    bool validateUsers(ull userID1, ull userID2) const;
    bool validatePost(ull postID) const;

    // =======================
    // Constructor & Destructor
    // =======================
    SystemManager();
    ~SystemManager();

    // =======================
    // System Lifecycle Management
    // =======================

    /**
     * Initialize all modules in correct dependency order
     * Returns: true if all modules initialized successfully
     */
    bool initializeAllModules();

    /**
     * Coordinate file I/O across all modules
     * Ensures consistent serialization format
     */
    bool saveAllData();
    bool loadAllData();

    /**
     * Graceful shutdown with cleanup
     */
    void shutdownAllModules();

    // =======================
    // USER OPERATIONS
    // =======================

    /**
     * Complete user registration workflow
     * 1. UserManager: Create user account
     * 2. RelationshipGraph: Register node
     * 3. StatusManager: Set offline initially
     * 4. SearchEngine: Index user for search
     * 5. MessageSystem: Register for messaging
     */
    bool registerUser(const std::string &uname,
                      const std::string &password,
                      const std::string &city);

    /**
     * Complete user deletion with cascading cleanup
     * 1. StatusManager: Remove status
     * 2. PostManager: Delete all user posts
     * 3. NotificationManager: Clear notifications (sent & received)
     * 4. FriendRequestManager: Cancel all requests
     * 5. RelationshipGraph: Remove all edges
     * 6. SearchEngine: Remove from index
     * 7. UserManager: Delete account
     */
    bool deleteUser(ull userID);

    /**
     * Complete login workflow
     * 1. UserManager: Validate credentials
     * 2. StatusManager: Mark online
     * 3. Return: unread notifications, initial feed, online friends
     */
    struct LoginResult
    {
        bool success;
        std::vector<Notification *> unreadNotifications;
        std::vector<Post *> initialFeed;
        std::vector<ull> onlineFriends;
    };
    LoginResult performCompleteLogin(ull userID, const std::string &password);

    /**
     * Complete logout workflow
     * 1. StatusManager: Mark offline
     * 2. UserManager: Log session end
     * 3. Clear active connections
     */
    bool performCompleteLogout(ull userID);

    // Basic user operations
    User *getUser(ull userID) const;
    User *getUserByUsername(const std::string &username) const;
    bool updateUserProfile(ull userID, const std::string &newCity);
    bool resetPassword(ull userID, const std::string &newPassword);

    // =======================
    // STATUS MANAGEMENT
    // =======================
    bool setUserOnline(ull userID);
    bool setUserOffline(ull userID);
    bool isUserOnline(ull userID) const;
    std::string getUserStatusString(ull userID) const;
    std::vector<ull> getOnlineFriends(ull userID) const;

    // =======================
    // POST OPERATIONS
    // =======================

    /**
     * Complete post creation workflow
     * 1. PostManager: Create post
     * 2. RelationshipGraph: Register post node
     * 3. SearchEngine: Index post content
     * 4. NotificationManager: Notify all followers
     */
    Post *performCreatePostComplete(ull authorID, const std::string &content);

    /**
     * Complete post deletion
     * 1. SearchEngine: Remove from index
     * 2. RelationshipGraph: Remove like edges
     * 3. PostManager: Delete post
     */
    bool deletePost(ull postID);

    Post *getPost(ull postID) const;
    std::vector<Post *> getUserPosts(ull userID) const;
    std::vector<Post *> getAllPosts() const;

    /**
     * Complete like workflow
     * 1. Validate post and user
     * 2. PostManager: Increment like count
     * 3. RelationshipGraph: Add like edge
     * 4. NotificationManager: Notify author
     * 5. Record activity for recommendations
     */
    bool performLikePostComplete(ull userID, ull postID);

    bool unlikePost(ull userID, ull postID);
    bool hasUserLikedPost(ull userID, ull postID) const;
    size_t getPostLikesCount(ull postID) const;

    // =======================
    // RELATIONSHIP OPERATIONS
    // =======================

    /**
     * Complete follow workflow
     * 1. Validate users
     * 2. RelationshipGraph: Add follow edge
     * 3. FollowerList: Update follower set
     * 4. NotificationManager: Send follow notification
     */
    bool performFollowWithNotification(ull followerID, ull followeeID);

    /**
     * Complete unfollow workflow
     * 1. RelationshipGraph: Remove edge
     * 2. FollowerList: Update sets
     * 3. FriendRequestManager: Cancel pending requests if any
     * 4. Clear active connections
     */
    bool performUnfollowComplete(ull followerID, ull followeeID);

    bool isFollowing(ull followerID, ull followeeID) const;

    std::vector<ull> getFollowers(ull userID) const;
    std::vector<ull> getFollowing(ull userID) const;
    size_t getFollowerCount(ull userID) const;
    size_t getFollowingCount(ull userID) const;

    std::vector<ull> getFriends(ull userID) const;
    std::vector<ull> getMutualConnections(ull userID) const;
    std::vector<ull> getMutualFriends(ull userID1, ull userID2) const;
    bool areUsersFriends(ull userID1, ull userID2) const;

    // =======================
    // FRIEND REQUEST OPERATIONS
    // =======================

    /**
     * Complete friend request workflow
     * 1. Validate: not already friends, no pending request
     * 2. Check: reverse request exists? (auto-accept)
     * 3. FriendRequestManager: Create request
     * 4. NotificationManager: Notify receiver
     */
    bool performSendFriendRequestComplete(ull senderID, ull receiverID);

    /**
     * Complete accept workflow
     * 1. FriendRequestManager: Mark accepted
     * 2. RelationshipGraph: Create bidirectional follow
     * 3. NotificationManager: Notify both users
     */
    bool performAcceptFriendRequest(ull receiverID, ull senderID);

    bool rejectFriendRequest(ull receiverID, ull senderID);
    bool cancelFriendRequest(ull senderID, ull receiverID);

    std::vector<FriendRequest> getPendingIncomingRequests(ull userID) const;
    std::vector<FriendRequest> getPendingOutgoingRequests(ull userID) const;
    bool hasPendingRequestFrom(ull receiverID, ull senderID) const;
    bool hasPendingRequestTo(ull senderID, ull receiverID) const;

    // =======================
    // MESSAGE OPERATIONS
    // =======================

    /**
     * Complete message send workflow
     * 1. Validate users via UserManager
     * 2. MessageSystem: Queue message
     * 3. NotificationManager: Send notification
     * 4. StatusManager: Check receiver online status
     * 5. Record activity for recommendations
     */
    bool performSendMessageComplete(ull senderID, ull receiverID,
                                    const std::string &content);

    std::vector<Message> getMessageHistory(ull user1ID, ull user2ID) const;
    Message getLatestMessage(ull user1ID, ull user2ID) const;
    bool markMessageAsRead(ull userID, ull messageID);

    /**
     * Search messages using KMP algorithm
     */
    std::vector<Message> searchMessages(ull userID, const std::string &keyword) const;

    // =======================
    // FEED OPERATIONS
    // =======================

    /**
     * Feed generation using merge-k-sorted-lists algorithm
     * 1. RelationshipGraph: Get following list
     * 2. PostManager: Fetch posts from each followed user
     * 3. FeedManager: Merge sorted post lists (by timestamp)
     * 4. Apply limit and return
     */
    std::vector<Post *> getUserFeed(ull userID, size_t limit = 50) const;
    std::vector<Post *> getUserFeedByFriends(ull userID, size_t limit = 50) const;
    std::vector<Post *> getUserFeedByMutualConnections(ull userID, size_t limit = 50) const;
    void displayUserFeed(ull userID, size_t limit = 50) const;

    // =======================
    // RECOMMENDATION OPERATIONS
    // =======================

    /**
     * User recommendations using BFS depth-2 traversal
     * Finds "friends of friends" and ranks by mutual connections
     */
    std::vector<ull> getRecommendedUsers(ull userID, size_t limit = 10) const;

    /**
     * Post recommendations based on friend network
     */
    std::vector<Post *> getRecommendedPosts(ull userID, size_t limit = 10) const;

    /**
     * Trending posts using top-k ranking by likes
     */
    std::vector<Post *> getTrendingPosts(size_t limit = 20) const;

    void displayRecommendedPosts(ull userID, size_t limit = 10) const;
    void displayRecommendedUsers(ull userID, size_t limit = 10) const;

    // =======================
    // SEARCH OPERATIONS
    // =======================

    /**
     * Multi-algorithm search engine:
     * - EXACT: Hash table lookup
     * - PREFIX: Trie-based search
     * - PARTIAL: KMP/Boyer-Moore string matching
     */
    std::vector<User *> searchUsers(const std::string &query,
                                    SearchMode mode = SearchMode::PARTIAL) const;
    std::vector<Post *> searchPosts(const std::string &query,
                                    SearchMode mode = SearchMode::PARTIAL) const;

    std::vector<User *> searchUsersExact(const std::string &query) const;
    std::vector<User *> searchUsersPrefix(const std::string &query) const;
    std::vector<Post *> searchPostsExact(const std::string &query) const;
    std::vector<Post *> searchPostsPrefix(const std::string &query) const;

    // =======================
    // NOTIFICATION OPERATIONS
    // =======================

    std::vector<Notification *> getAllNotifications(ull userID) const;
    std::vector<Notification *> getUnreadNotifications(ull userID) const;
    std::vector<Notification *> getNotificationsByType(ull userID,
                                                       NotificationType type) const;
    Notification *getNotificationByID(ull notificationID) const;

    size_t getUnreadNotificationCount(ull userID) const;
    bool markNotificationAsRead(ull notificationID);
    bool markAllNotificationsAsRead(ull userID);
    bool deleteNotification(ull notificationID);

    // Notification creation for various events
    void notifyLike(ull receiverID, ull senderID, ull postID);
    void notifyFollow(ull receiverID, ull senderID);
    void notifyMessage(ull receiverID, ull senderID);
    void notifyFriendRequest(ull receiverID, ull senderID);
    void notifySystem(ull receiverID, const std::string &description);

    // =======================
    // ACTIVITY TRACKING
    // =======================

    /**
     * Record interactions for recommendation algorithm
     */
    void recordActivity(ull user1ID, ull user2ID);
    void expireOldActivities();
    std::vector<ull> getActiveConnections(ull userID) const;
    void clearAllActivities();

    // =======================
    // ANALYTICS & STATISTICS
    // =======================

    struct UserStats
    {
        size_t followerCount;
        size_t followingCount;
        size_t friendCount;
        size_t postCount;
        size_t totalLikes;
        size_t unreadNotifications;
        size_t pendingRequests;
        bool isOnline;
    };

    /**
     * Get comprehensive user statistics
     * Aggregates data from multiple modules
     */
    UserStats getUserStats(ull userID) const;

    // Bulk cleanup operations
    void clearUserNotifications(ull userID);
    void clearUserFriendRequests(ull userID);

    // =======================
    // VALIDATION HELPERS
    // =======================

    bool canFollowUser(ull followerID, ull followeeID) const;
    bool canSendFriendRequest(ull senderID, ull receiverID) const;

    // =======================
    // DISPLAY & DEBUG
    // =======================

    void displayAllUsers() const;
    void displayUserProfile(ull userID) const;
    void displayAllPosts() const;
    void displaySystemStatus() const;
};