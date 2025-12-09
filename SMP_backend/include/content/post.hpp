#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <fstream>
#include "nlohmann/json.hpp"
#include "ADT/hash_map.hpp"

typedef unsigned long long ull;
using json = nlohmann::json;

class Post
{
private:
    ull postID;
    ull authorID;
    std::string content;
    std::time_t timestamp;
    std::vector<ull> likes;

    static ull nextpID;

public:
    Post(ull id, ull author, const std::string &text);

    ull getPostID() const;
    ull getAuthor() const;
    const std::string &getContent() const;
    std::time_t getTimestamp() const;
    size_t getLikesCount() const;

    void setContent(const std::string &newContent);
    void addLikes(ull userID);
    void removeLike(ull userID);

    json toJSON() const;
    static Post fromJSON(const json &j);

    void display() const;
};

// Linked List Node for Posts
struct PostNode
{
    Post *post;
    PostNode *next;

    PostNode(Post *p) : post(p), next(nullptr) {}
};

// Linked List for managing user's posts
class PostList
{
private:
    PostNode *head;
    size_t count;

public:
    PostList();
    ~PostList();

    void addPost(Post *p);                   // Add to front (newest first)
    bool removePost(ull postID);             // Remove by ID
    Post *findPost(ull postID) const;        // Find post by ID
    std::vector<Post *> getAllPosts() const; // Get all posts in order
    size_t size() const;
    void clear();

    // For serialization
    PostNode *getHead() const { return head; }
};

class PostManager
{
private:
    // Primary DS: userID → Linked List of Posts
    HashMap<ull, PostList *> userPosts;

    // Secondary index for fast post lookup by postID
    HashMap<ull, Post *> postIndex;

    ull nextPostID;
    std::string filePath;

    // Edit time window in seconds (e.g., 300 = 5 minutes)
    static const int EDIT_WINDOW = 300;

public:
    PostManager(const std::string &path);
    ~PostManager();

    // Core Responsibilities
    Post *createPost(ull authorID, const std::string &content);
    bool deletePost(ull postID);
    bool editPost(ull postID, const std::string &newContent); // Time-restricted
    Post *getPost(ull postID) const;

    // View Posts
    std::vector<Post *> getPostsByUser(ull userID) const; // Sorted newest first
    std::vector<Post *> getAllPosts() const;

    // Search Within Posts (using KMP algorithm)
    std::vector<Post *> searchPosts(const std::string &keyword) const;
    std::vector<Post *> searchPostsByUser(ull userID, const std::string &keyword) const;

    // Persistence
    bool loadFromFile();
    bool saveToFile() const;

    // Utility
    void displayAll() const;
    bool canEdit(ull postID) const; // Check if post is within edit window
};