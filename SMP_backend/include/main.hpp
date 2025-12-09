#include "system/SystemManager.hpp"
#include <iostream>
#include <string>
#include <limits>

// ═══════════════════════════════════════
// MENU DISPLAY FUNCTIONS
// ═══════════════════════════════════════

// Display the main menu for non-logged-in users
void displayMainMenu();

// Display the user dashboard for logged-in users
void displayUserMenu(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// USER AUTHENTICATION HANDLERS
// ═══════════════════════════════════════

// Handle user registration workflow
void handleUserRegistration(SystemManager *sysMgr);

// Handle user login workflow and set currentUserID on success
void handleUserLogin(SystemManager *sysMgr, ull &currentUserID);

// Handle user logout workflow
void handleUserLogout(SystemManager *sysMgr, ull &currentUserID);

// ═══════════════════════════════════════
// POST MANAGEMENT HANDLERS
// ═══════════════════════════════════════

// Handle post creation
void handleCreatePost(SystemManager *sysMgr, ull currentUserID);

// Handle viewing user feed with options
void handleViewFeed(SystemManager *sysMgr, ull currentUserID);

// Handle viewing user's own posts
void handleViewMyPosts(SystemManager *sysMgr, ull currentUserID);

// Handle post deletion
void handleDeletePost(SystemManager *sysMgr, ull currentUserID);

// Handle liking a post
void handleLikePost(SystemManager *sysMgr, ull currentUserID);

// Handle unliking a post
void handleUnlikePost(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// SOCIAL INTERACTION HANDLERS
// ═══════════════════════════════════════

// Main social menu handler
void handleSocialMenu(SystemManager *sysMgr, ull currentUserID);

// Handle following a user
void handleFollowUser(SystemManager *sysMgr, ull currentUserID);

// Handle unfollowing a user
void handleUnfollowUser(SystemManager *sysMgr, ull currentUserID);

// Handle viewing followers list
void handleViewFollowers(SystemManager *sysMgr, ull currentUserID);

// Handle viewing following list
void handleViewFollowing(SystemManager *sysMgr, ull currentUserID);

// Handle viewing friends list
void handleViewFriends(SystemManager *sysMgr, ull currentUserID);

// Handle viewing mutual friends with another user
void handleViewMutualFriends(SystemManager *sysMgr, ull currentUserID);

// Handle viewing online friends
void handleViewOnlineFriends(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// MESSAGING HANDLERS
// ═══════════════════════════════════════

// Main messaging menu handler
void handleMessagingMenu(SystemManager *sysMgr, ull currentUserID);

// Handle sending a message
void handleSendMessage(SystemManager *sysMgr, ull currentUserID);

// Handle viewing conversation with a user
void handleViewConversation(SystemManager *sysMgr, ull currentUserID);

// Handle searching through messages
void handleSearchMessages(SystemManager *sysMgr, ull currentUserID);

// Handle marking message as read
void handleMarkMessageRead(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// NOTIFICATION HANDLERS
// ═══════════════════════════════════════

// Main notifications menu handler
void handleNotificationsMenu(SystemManager *sysMgr, ull currentUserID);

// Handle viewing all notifications
void handleViewAllNotifications(SystemManager *sysMgr, ull currentUserID);

// Handle viewing unread notifications
void handleViewUnreadNotifications(SystemManager *sysMgr, ull currentUserID);

// Handle viewing notifications by type
void handleViewNotificationsByType(SystemManager *sysMgr, ull currentUserID);

// Handle marking notification as read
void handleMarkNotificationRead(SystemManager *sysMgr, ull currentUserID);

// Handle marking all notifications as read
void handleMarkAllNotificationsRead(SystemManager *sysMgr, ull currentUserID);

// Handle deleting a notification
void handleDeleteNotification(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// FRIEND REQUEST HANDLERS
// ═══════════════════════════════════════

// Main friend request menu handler
void handleFriendRequestMenu(SystemManager *sysMgr, ull currentUserID);

// Handle sending a friend request
void handleSendFriendRequest(SystemManager *sysMgr, ull currentUserID);

// Handle viewing incoming friend requests
void handleViewIncomingRequests(SystemManager *sysMgr, ull currentUserID);

// Handle viewing outgoing friend requests
void handleViewOutgoingRequests(SystemManager *sysMgr, ull currentUserID);

// Handle accepting a friend request
void handleAcceptFriendRequest(SystemManager *sysMgr, ull currentUserID);

// Handle rejecting a friend request
void handleRejectFriendRequest(SystemManager *sysMgr, ull currentUserID);

// Handle canceling an outgoing friend request
void handleCancelFriendRequest(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// SEARCH HANDLERS
// ═══════════════════════════════════════

// Main search menu handler
void handleSearchMenu(SystemManager *sysMgr, ull currentUserID);

// Handle searching for users
void handleSearchUsers(SystemManager *sysMgr, ull currentUserID);

// Handle searching for posts
void handleSearchPosts(SystemManager *sysMgr, ull currentUserID);

// Handle exact user search
void handleSearchUsersExact(SystemManager *sysMgr, ull currentUserID);

// Handle prefix user search
void handleSearchUsersPrefix(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// RECOMMENDATION HANDLERS
// ═══════════════════════════════════════

// Main recommendations menu handler
void handleRecommendationsMenu(SystemManager *sysMgr, ull currentUserID);

// Handle viewing recommended posts
void handleViewRecommendedPosts(SystemManager *sysMgr, ull currentUserID);

// Handle viewing recommended users
void handleViewRecommendedUsers(SystemManager *sysMgr, ull currentUserID);

// Handle viewing trending posts
void handleViewTrendingPosts(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// PROFILE HANDLERS
// ═══════════════════════════════════════

// Main profile menu handler
void handleProfileMenu(SystemManager *sysMgr, ull currentUserID);

// Handle viewing full user profile
void handleViewProfile(SystemManager *sysMgr, ull currentUserID);

// Handle viewing another user's profile
void handleViewOtherProfile(SystemManager *sysMgr, ull currentUserID);

// Handle updating user city
void handleUpdateCity(SystemManager *sysMgr, ull currentUserID);

// Handle changing password
void handleChangePassword(SystemManager *sysMgr, ull currentUserID);

// Handle viewing user statistics
void handleViewUserStats(SystemManager *sysMgr, ull currentUserID);

// ═══════════════════════════════════════
// ADMIN HANDLERS
// ═══════════════════════════════════════

// Main admin menu handler
void handleAdminMenu(SystemManager *sysMgr);

// Handle viewing all users
void handleViewAllUsers(SystemManager *sysMgr);

// Handle viewing all posts
void handleViewAllPosts(SystemManager *sysMgr);

// Handle viewing system status
void handleViewSystemStatus(SystemManager *sysMgr);

// Handle manual data save
void handleSaveData(SystemManager *sysMgr);

// Handle viewing specific user profile (admin)
void handleAdminViewUserProfile(SystemManager *sysMgr);

// Handle deleting a user (admin)
void handleAdminDeleteUser(SystemManager *sysMgr);

// ═══════════════════════════════════════
// UTILITY FUNCTIONS
// ═══════════════════════════════════════

// Clear the console screen
void clearScreen();

// Pause screen and wait for user input
void pauseScreen();

// Display welcome banner
void displayWelcomeBanner();

// Display goodbye message
void displayGoodbyeMessage();

// Get valid integer input from user
int getIntInput(const std::string &prompt);

// Get string input from user
std::string getStringInput(const std::string &prompt);

// Display success message
void displaySuccess(const std::string &message);

// Display error message
void displayError(const std::string &message);

// Display info message
void displayInfo(const std::string &message);

// Confirm action with user (yes/no)
bool confirmAction(const std::string &prompt);

// ═══════════════════════════════════════
// MAIN FUNCTION
// ═══════════════════════════════════════

int main();