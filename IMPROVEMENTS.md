# Improvement Roadmap & Scaling Guide

## Overview

This document outlines potential improvements and scaling strategies for the Mini Instagram project. It's designed to help developers understand how to evolve this educational project into a more robust application.

## Short-term Improvements (1-2 weeks)

### 1. Code Quality Enhancements

#### Fix Compiler Warnings
- [ ] Remove unused variables in `systemManager.cpp`
- [ ] Handle all enum cases in switch statements
- [ ] Fix initialization order warnings
- [ ] Add `[[maybe_unused]]` attribute where appropriate

#### Add Const Correctness
```cpp
// Before
User* getUser(ull userID);

// After
const User* getUser(ull userID) const;
User* getUser(ull userID);
```

#### Memory Management
- [ ] Add RAII wrappers for all dynamic allocations
- [ ] Use smart pointers where appropriate
- [ ] Run valgrind to detect memory leaks
- [ ] Implement proper copy constructors and assignment operators

### 2. Testing Infrastructure

#### Unit Testing Framework
```bash
# Install Google Test
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make
sudo make install
```

#### Example Test Structure
```cpp
// tests/test_hashmap.cpp
#include <gtest/gtest.h>
#include "ADT/hash_map.hpp"

class HashMapTest : public ::testing::Test {
protected:
    HashMap<int, std::string> map;
    
    void SetUp() override {
        map.insert(1, "one");
        map.insert(2, "two");
    }
};

TEST_F(HashMapTest, InsertAndRetrieve) {
    EXPECT_TRUE(map.contains(1));
    EXPECT_EQ(map.get(1), "one");
}

TEST_F(HashMapTest, RemoveElement) {
    map.erase(1);
    EXPECT_FALSE(map.contains(1));
}
```

#### Integration Testing
```cpp
// tests/test_workflows.cpp
TEST(WorkflowTest, CreatePostWorkflow) {
    SystemManager mgr;
    mgr.initializeAllModules();
    
    // Register user
    ASSERT_TRUE(mgr.registerUser("testuser", "password123", "TestCity"));
    
    // Login
    auto result = mgr.performCompleteLogin(1, "password123");
    ASSERT_TRUE(result.success);
    
    // Create post
    Post* post = mgr.performCreatePostComplete(1, "Test post content");
    ASSERT_NE(post, nullptr);
    EXPECT_EQ(post->getContent(), "Test post content");
}
```

### 3. Documentation Improvements

#### Inline Documentation
```cpp
/**
 * @brief Creates a new post and notifies followers
 * 
 * This method performs the complete workflow for post creation:
 * 1. Validates user permissions
 * 2. Creates the post object
 * 3. Updates the feed cache
 * 4. Notifies all followers
 * 
 * @param userID The ID of the user creating the post
 * @param content The text content of the post
 * @return Pointer to the created Post, or nullptr on failure
 * 
 * @throws std::invalid_argument if content is empty
 * @throws std::runtime_error if user doesn't exist
 * 
 * @note This operation is O(f) where f is the number of followers
 * 
 * @example
 * Post* myPost = mgr.performCreatePostComplete(123, "Hello World!");
 * if (myPost) {
 *     std::cout << "Post created with ID: " << myPost->getPostID() << "\n";
 * }
 */
Post* performCreatePostComplete(ull userID, const std::string& content);
```

#### Generate API Documentation
```bash
# Install Doxygen
sudo apt-get install doxygen graphviz

# Create Doxyfile
doxygen -g

# Configure Doxyfile for the project
# Then generate docs
doxygen Doxyfile

# View documentation
xdg-open html/index.html
```

## Medium-term Improvements (1-2 months)

### 1. Feature Additions

#### Comments System
```cpp
class Comment {
private:
    ull commentID;
    ull postID;
    ull authorID;
    std::string content;
    time_t timestamp;
    ull parentCommentID; // For nested comments
    
public:
    Comment(ull pid, ull aid, const std::string& text);
    // ... methods
};

class CommentManager {
private:
    HashMap<ull, Comment*> comments;
    HashMap<ull, std::vector<ull>> postComments; // postID -> commentIDs
    
public:
    Comment* addComment(ull postID, ull userID, const std::string& text);
    std::vector<Comment*> getPostComments(ull postID);
    bool deleteComment(ull commentID);
};
```

#### Hashtag System
```cpp
class HashtagManager {
private:
    Trie hashtagIndex;
    HashMap<std::string, std::vector<ull>> hashtagPosts; // hashtag -> postIDs
    HashMap<ull, std::vector<std::string>> postHashtags; // postID -> hashtags
    
public:
    void indexPost(ull postID, const std::string& content);
    std::vector<ull> getPostsByHashtag(const std::string& hashtag);
    std::vector<std::string> getTrendingHashtags(size_t limit = 10);
};
```

#### Media Support (Basic)
```cpp
class MediaManager {
private:
    std::string mediaDirectory;
    HashMap<ull, std::vector<std::string>> postMedia; // postID -> file paths
    
public:
    bool uploadImage(ull postID, const std::string& imagePath);
    std::vector<std::string> getPostMedia(ull postID);
    bool deleteMedia(ull postID, const std::string& mediaPath);
    
private:
    bool validateImageFile(const std::string& path);
    std::string generateMediaPath(ull postID);
};
```

#### Group/Community Features
```cpp
class Group {
private:
    ull groupID;
    std::string name;
    std::string description;
    ull ownerID;
    Set<ull> members;
    Set<ull> admins;
    bool isPrivate;
    
public:
    bool addMember(ull userID);
    bool removeMember(ull userID);
    bool isMember(ull userID) const;
};

class GroupManager {
private:
    HashMap<ull, Group*> groups;
    HashMap<ull, std::vector<ull>> userGroups; // userID -> groupIDs
    
public:
    Group* createGroup(ull ownerID, const std::string& name);
    bool joinGroup(ull userID, ull groupID);
    std::vector<Group*> getUserGroups(ull userID);
};
```

### 2. Performance Optimizations

#### Caching Layer
```cpp
class FeedCache {
private:
    HashMap<ull, std::vector<Post*>> cachedFeeds;
    HashMap<ull, time_t> cacheTimestamps;
    const int CACHE_TTL = 300; // 5 minutes
    
public:
    bool isCacheValid(ull userID) {
        if (!cacheTimestamps.contains(userID)) return false;
        return (time(nullptr) - cacheTimestamps[userID]) < CACHE_TTL;
    }
    
    std::vector<Post*> getCachedFeed(ull userID) {
        if (isCacheValid(userID)) {
            return cachedFeeds[userID];
        }
        return {};
    }
    
    void updateCache(ull userID, const std::vector<Post*>& feed) {
        cachedFeeds[userID] = feed;
        cacheTimestamps[userID] = time(nullptr);
    }
    
    void invalidate(ull userID) {
        cacheTimestamps.erase(userID);
        cachedFeeds.erase(userID);
    }
};
```

#### Lazy Loading
```cpp
class LazyPostLoader {
public:
    std::vector<Post*> getPostsBatch(ull userID, size_t offset, size_t limit) {
        // Only load 'limit' posts starting from 'offset'
        // Reduces memory usage for large feeds
        std::vector<Post*> batch;
        // ... implementation
        return batch;
    }
};
```

#### Index Optimization
```cpp
// Add secondary indices for common queries
class OptimizedUserManager {
private:
    HashMap<ull, User*> usersByID;           // Primary index
    HashMap<std::string, ull> usernameIndex;  // Secondary index
    HashMap<std::string, std::vector<ull>> cityIndex; // City search
    
public:
    User* getUserByUsername(const std::string& username) {
        if (usernameIndex.contains(username)) {
            return usersByID[usernameIndex[username]];
        }
        return nullptr;
    }
    
    std::vector<User*> getUsersByCity(const std::string& city) {
        std::vector<User*> users;
        if (cityIndex.contains(city)) {
            for (ull id : cityIndex[city]) {
                users.push_back(usersByID[id]);
            }
        }
        return users;
    }
};
```

### 3. Configuration System

#### JSON Configuration
```cpp
// config.json
{
    "database": {
        "type": "file",
        "path": "./data",
        "auto_save_interval": 300
    },
    "security": {
        "password_min_length": 8,
        "max_login_attempts": 5,
        "session_timeout": 3600
    },
    "features": {
        "enable_notifications": true,
        "enable_recommendations": true,
        "max_posts_per_day": 50
    },
    "performance": {
        "cache_enabled": true,
        "cache_ttl": 300,
        "max_feed_size": 100
    }
}
```

```cpp
// include/utils/config.hpp
#include "nlohmann/json.hpp"

class Config {
private:
    nlohmann::json config;
    static Config* instance;
    
    Config() {
        loadFromFile("config.json");
    }
    
public:
    static Config* getInstance() {
        if (!instance) instance = new Config();
        return instance;
    }
    
    template<typename T>
    T get(const std::string& path) {
        // Parse dot notation: "database.path"
        // Return default if not found
    }
    
    void loadFromFile(const std::string& filename);
    void save();
};
```

## Long-term Improvements (3-6 months)

### 1. Database Integration

#### SQLite Migration
```cpp
// Use SQLite for persistent storage
#include <sqlite3.h>

class DatabaseManager {
private:
    sqlite3* db;
    
public:
    bool initialize(const std::string& dbPath) {
        return sqlite3_open(dbPath.c_str(), &db) == SQLITE_OK;
    }
    
    bool executeQuery(const std::string& query) {
        char* errMsg;
        int rc = sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "SQL error: " << errMsg << "\n";
            sqlite3_free(errMsg);
            return false;
        }
        return true;
    }
    
    // Prepared statements for security
    std::vector<User*> getUsersByCity(const std::string& city) {
        sqlite3_stmt* stmt;
        const char* sql = "SELECT * FROM users WHERE city = ?";
        sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        sqlite3_bind_text(stmt, 1, city.c_str(), -1, SQLITE_STATIC);
        
        std::vector<User*> users;
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            // Create User objects from results
        }
        sqlite3_finalize(stmt);
        return users;
    }
};
```

#### Schema Design
```sql
-- users.sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password_hash VARCHAR(255) NOT NULL,
    salt VARCHAR(64) NOT NULL,
    city VARCHAR(100),
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_login TIMESTAMP,
    is_online BOOLEAN DEFAULT 0
);

CREATE INDEX idx_username ON users(username);
CREATE INDEX idx_city ON users(city);

-- posts.sql
CREATE TABLE posts (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    author_id INTEGER NOT NULL,
    content TEXT NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    likes_count INTEGER DEFAULT 0,
    FOREIGN KEY (author_id) REFERENCES users(id) ON DELETE CASCADE
);

CREATE INDEX idx_author ON posts(author_id);
CREATE INDEX idx_created ON posts(created_at DESC);

-- relationships.sql
CREATE TABLE follows (
    follower_id INTEGER NOT NULL,
    following_id INTEGER NOT NULL,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (follower_id, following_id),
    FOREIGN KEY (follower_id) REFERENCES users(id) ON DELETE CASCADE,
    FOREIGN KEY (following_id) REFERENCES users(id) ON DELETE CASCADE
);

CREATE INDEX idx_follower ON follows(follower_id);
CREATE INDEX idx_following ON follows(following_id);
```

### 2. RESTful API Layer

#### HTTP Server with cpp-httplib
```cpp
#include <httplib.h>

class APIServer {
private:
    httplib::Server server;
    SystemManager* sysMgr;
    
public:
    APIServer(SystemManager* mgr) : sysMgr(mgr) {
        setupRoutes();
    }
    
    void setupRoutes() {
        // User routes
        server.Post("/api/users/register", [this](const auto& req, auto& res) {
            handleRegister(req, res);
        });
        
        server.Post("/api/users/login", [this](const auto& req, auto& res) {
            handleLogin(req, res);
        });
        
        // Post routes
        server.Get("/api/posts/:id", [this](const auto& req, auto& res) {
            handleGetPost(req, res);
        });
        
        server.Post("/api/posts", [this](const auto& req, auto& res) {
            handleCreatePost(req, res);
        });
        
        // Feed routes
        server.Get("/api/feed", [this](const auto& req, auto& res) {
            handleGetFeed(req, res);
        });
    }
    
    void start(int port = 8080) {
        std::cout << "Starting API server on port " << port << "...\n";
        server.listen("0.0.0.0", port);
    }
    
private:
    void handleRegister(const httplib::Request& req, httplib::Response& res) {
        auto json = nlohmann::json::parse(req.body);
        std::string username = json["username"];
        std::string password = json["password"];
        std::string city = json["city"];
        
        if (sysMgr->registerUser(username, password, city)) {
            res.set_content(R"({"success": true})", "application/json");
            res.status = 201;
        } else {
            res.set_content(R"({"error": "Registration failed"})", "application/json");
            res.status = 400;
        }
    }
};
```

### 3. Real-time Features

#### WebSocket Support
```cpp
// Use websocketpp for real-time messaging
#include <websocketpp/server.hpp>

class RealtimeServer {
private:
    websocketpp::server<websocketpp::config::asio> wsServer;
    HashMap<ull, websocketpp::connection_hdl> activeConnections;
    
public:
    void onMessage(websocketpp::connection_hdl hdl, 
                   websocketpp::server<websocketpp::config::asio>::message_ptr msg) {
        // Handle incoming messages
        auto payload = nlohmann::json::parse(msg->get_payload());
        
        if (payload["type"] == "message") {
            ull recipientID = payload["recipient_id"];
            if (activeConnections.contains(recipientID)) {
                // Send message to recipient in real-time
                wsServer.send(activeConnections[recipientID], 
                             msg->get_payload(), 
                             websocketpp::frame::opcode::text);
            }
        }
    }
    
    void notifyUser(ull userID, const std::string& notification) {
        if (activeConnections.contains(userID)) {
            nlohmann::json msg = {
                {"type", "notification"},
                {"content", notification}
            };
            wsServer.send(activeConnections[userID], 
                         msg.dump(), 
                         websocketpp::frame::opcode::text);
        }
    }
};
```

### 4. Microservices Architecture

#### Service Separation
```
┌─────────────────────────────────────────────────────────────┐
│                    API Gateway (NGINX)                       │
└─────────────────────────────────────────────────────────────┘
         │                │                │               │
         ▼                ▼                ▼               ▼
┌──────────────┐  ┌──────────────┐  ┌──────────────┐  ┌────────────┐
│   Auth       │  │   User       │  │   Post       │  │  Message   │
│   Service    │  │   Service    │  │   Service    │  │  Service   │
└──────────────┘  └──────────────┘  └──────────────┘  └────────────┘
         │                │                │               │
         └────────────────┴────────────────┴───────────────┘
                              │
                              ▼
                     ┌──────────────────┐
                     │   PostgreSQL     │
                     │   Database       │
                     └──────────────────┘
```

#### Inter-service Communication
```cpp
// Use gRPC for service-to-service communication
#include <grpcpp/grpcpp.h>

// user_service.proto
service UserService {
    rpc GetUser(UserRequest) returns (UserResponse) {}
    rpc CreateUser(CreateUserRequest) returns (UserResponse) {}
}

// Implementation
class UserServiceImpl final : public UserService::Service {
    grpc::Status GetUser(grpc::ServerContext* context,
                        const UserRequest* request,
                        UserResponse* response) override {
        // Fetch user and populate response
        return grpc::Status::OK;
    }
};
```

## Scaling Strategies

### Horizontal Scaling
- Load balancer (NGINX, HAProxy)
- Multiple application instances
- Session storage in Redis
- Database replication

### Vertical Scaling
- Increase server resources
- Optimize algorithms
- Better caching strategies

### Database Scaling
- Read replicas for query distribution
- Sharding by user ID
- Caching layer (Redis/Memcached)
- Connection pooling

### Content Delivery
- CDN for media files
- Image optimization and compression
- Lazy loading strategies

## Performance Benchmarking

```cpp
// Benchmark utility
#include <chrono>

class Benchmark {
public:
    template<typename Func>
    static double measure(Func&& func, int iterations = 1000) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < iterations; ++i) {
            func();
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        return duration.count() / static_cast<double>(iterations);
    }
};

// Usage
double avgTime = Benchmark::measure([]() {
    // Code to benchmark
    mgr.getUserFeed(123, 50);
});

std::cout << "Average time: " << avgTime << " μs\n";
```

## Monitoring and Observability

### Logging Framework
```cpp
enum LogLevel { DEBUG, INFO, WARNING, ERROR, CRITICAL };

class Logger {
public:
    static void log(LogLevel level, const std::string& message) {
        std::ofstream logFile("logs/app.log", std::ios::app);
        logFile << getCurrentTimestamp() << " | "
                << levelToString(level) << " | "
                << message << "\n";
    }
};
```

### Metrics Collection
```cpp
class Metrics {
private:
    std::atomic<long> requestCount{0};
    std::atomic<long> errorCount{0};
    
public:
    void recordRequest() { ++requestCount; }
    void recordError() { ++errorCount; }
    
    void export() {
        // Export to Prometheus, Grafana, etc.
    }
};
```

## Deployment

### Docker Containerization
```dockerfile
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    g++ \
    make \
    cmake \
    libsqlite3-dev

WORKDIR /app
COPY . /app

RUN cd SMP_backend && make release

EXPOSE 8080

CMD ["./SMP_backend/build/Mini_Instagram"]
```

### Docker Compose
```yaml
version: '3.8'

services:
  app:
    build: .
    ports:
      - "8080:8080"
    environment:
      - DATABASE_PATH=/data/db
      - LOG_LEVEL=INFO
    volumes:
      - ./data:/data
    depends_on:
      - db
      - redis
  
  db:
    image: postgres:15
    environment:
      POSTGRES_PASSWORD: secret
    volumes:
      - pgdata:/var/lib/postgresql/data
  
  redis:
    image: redis:7
    
volumes:
  pgdata:
```

## Conclusion

This roadmap provides a structured approach to improving and scaling the Mini Instagram project. Start with short-term improvements to build a solid foundation, then gradually implement medium and long-term enhancements based on your needs and timeline.

Remember: **Premature optimization is the root of all evil.** Focus on correctness first, then performance.
