# Architecture Documentation

## System Overview

Mini Instagram is a social media platform backend built with C++ that demonstrates the practical application of various data structures and algorithms. The system follows a modular architecture with clear separation of concerns.

## Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        Main Application CLI                      │
│                         (main.cpp)                              │
└───────────────────────────┬─────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                      System Manager                              │
│              (Central Orchestration Layer)                       │
│  - Module Lifecycle Management                                   │
│  - Workflow Orchestration                                        │
│  - Data Integrity Control                                        │
└────┬──────┬──────┬──────┬──────┬──────┬──────┬──────┬──────────┘
     │      │      │      │      │      │      │      │
     ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼
┌────────┬─────┬──────┬──────┬──────┬──────┬──────┬─────────┐
│  User  │Post │Feed  │Rela- │Status│Search│Notif │Message  │
│Manager │ Mgr │ Mgr  │tion  │ Mgr  │Engine│ Mgr  │ System  │
│        │     │      │Graph │      │      │      │         │
└────────┴─────┴──────┴──────┴──────┴──────┴──────┴─────────┘
     │      │      │      │      │      │      │      │
     ▼      ▼      ▼      ▼      ▼      ▼      ▼      ▼
┌─────────────────────────────────────────────────────────────────┐
│                    Data Structures Layer                         │
│  ┌──────────┬──────────┬──────────┬──────────┬──────────┐      │
│  │ HashMap  │  Graph   │   Trie   │  Queue   │   Set    │      │
│  └──────────┴──────────┴──────────┴──────────┴──────────┘      │
└─────────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────────┐
│                     File I/O Layer                              │
│                  (Data Persistence)                             │
└─────────────────────────────────────────────────────────────────┘
```

## Layer Description

### 1. Presentation Layer (CLI)
**Files**: `src/main.cpp`

**Responsibilities**:
- User interface rendering
- Input collection and validation
- Menu navigation
- Display formatting and colors

**Key Classes**:
- `SocialPlatformCLI`: Main application class

### 2. Orchestration Layer (System Manager)
**Files**: `include/system/SystemManager.hpp`, `src/system/systemManager.cpp`

**Responsibilities**:
- Coordinate interactions between modules
- Manage complex workflows (login, post creation, friend requests)
- Ensure data consistency across modules
- Handle module initialization and shutdown
- Data persistence coordination

**Design Pattern**: Facade Pattern - Provides simplified interface to complex subsystems

### 3. Business Logic Layer

#### Core Module
**Files**: `include/core/*`, `src/core/*`

**Components**:
- **UserManager**: User authentication, profile management
- **RelationshipGraph**: Social graph (follow/friend relationships)
- **StatusManager**: Online/offline user status tracking
- **FollowerList**: Efficient follower/following queries

**Data Structures Used**:
- HashMap for user lookup (O(1) average)
- Graph for relationships (O(1) for edge operations)
- Set for unique collections

#### Content Module
**Files**: `include/content/*`, `src/content/*`

**Components**:
- **PostManager**: Post CRUD operations
- **FeedManager**: Personalized feed generation
- **RecommendationEngine**: User and post recommendations

**Algorithms**:
- Feed ranking based on recency and relevance
- Friend-of-friend recommendations
- Engagement-based post ranking

#### Interaction Module
**Files**: `include/interaction/*`, `src/interaction/*`

**Components**:
- **MessageSystem**: Direct messaging
- **FriendRequestManager**: Friend request workflows
- **NotificationManager**: Notification delivery

**Data Structures Used**:
- Queue for message delivery
- HashMap for fast message lookup

#### System Module
**Files**: `include/system/*`, `src/system/*`

**Components**:
- **SearchEngine**: User and post search
- **NotificationManager**: System notifications

**Data Structures Used**:
- Trie for prefix-based searching (O(k) where k is key length)

### 4. Data Structures Layer
**Files**: `include/ADT/*`

**Custom Implementations**:

| Structure | Implementation | Use Cases |
|-----------|---------------|-----------|
| **HashMap** | Chaining with linked lists | User lookup, post storage |
| **Graph** | Adjacency list (dual maps) | Social relationships, likes |
| **Trie** | Tree-based prefix search | Username/content search |
| **Queue** | Linked list based | Messages, notifications |
| **Set** | Hash-based | Unique collections, likes |
| **LinkedList** | Singly linked | General collections |
| **Stack** | Array-based | Future undo operations |

### 5. Utility Layer
**Files**: `include/utils/*`, `src/utils/*`

**Components**:
- **FileIO**: Data serialization/deserialization
- **Validation**: Input validation
- **Helpers**: Common utility functions

## Data Flow Examples

### Example 1: User Login Workflow

```
1. CLI: User enters credentials
   ↓
2. SystemManager.performCompleteLogin(username, password)
   ↓
3. UserManager.authenticateUser(username, password)
   ├─→ Validate credentials (O(1) HashMap lookup)
   └─→ Return User object
   ↓
4. StatusManager.setUserOnline(userID)
   ↓
5. NotificationManager.getUnreadNotifications(userID)
   ↓
6. RelationshipGraph.getOnlineFriends(userID)
   ↓
7. SystemManager: Aggregate results
   ↓
8. CLI: Display dashboard with stats
```

### Example 2: Create Post Workflow

```
1. CLI: User enters post content
   ↓
2. SystemManager.performCreatePostComplete(userID, content)
   ↓
3. PostManager.createPost(userID, content)
   ├─→ Generate unique post ID
   ├─→ Create Post object
   └─→ Store in HashMap (O(1))
   ↓
4. FeedManager.invalidateCache(userID)
   ↓
5. RelationshipGraph.getFollowers(userID)
   ↓
6. NotificationManager.notifyFollowers(userID, postID)
   ├─→ For each follower:
   │   └─→ Queue notification (O(1))
   ↓
7. FileIO.savePostData()
   ↓
8. CLI: Confirm post creation
```

### Example 3: Search Users Workflow

```
1. CLI: User enters search query
   ↓
2. SystemManager.searchUsersPrefix(query)
   ↓
3. SearchEngine.prefixSearch(query)
   ├─→ Traverse Trie (O(k) where k = query length)
   └─→ Return matching user IDs
   ↓
4. UserManager.getUserDetails(userIDs)
   ├─→ Fetch from HashMap (O(1) per user)
   └─→ Return User objects
   ↓
5. CLI: Display search results
```

## Key Design Decisions

### 1. Custom Data Structures
**Decision**: Implement all data structures from scratch
**Rationale**: Educational project to demonstrate DS concepts
**Trade-off**: More development time, potential bugs vs. learning value

### 2. File-based Storage
**Decision**: Use text files for persistence
**Rationale**: Simple, portable, no external dependencies
**Trade-off**: Limited scalability, no transaction support

### 3. Modular Architecture
**Decision**: Separate concerns into distinct modules
**Rationale**: Maintainability, testability, clear responsibility
**Trade-off**: More files/classes vs. better organization

### 4. SystemManager as Facade
**Decision**: Central orchestrator for cross-module operations
**Rationale**: Simplify complex workflows, ensure consistency
**Trade-off**: Potential tight coupling vs. simplified interface

## Performance Characteristics

### Time Complexity

| Operation | Complexity | Data Structure |
|-----------|-----------|----------------|
| User Login | O(1) avg | HashMap |
| Create Post | O(1) | HashMap + Graph |
| Get Feed | O(f × p) | where f=friends, p=posts/friend |
| Follow User | O(1) avg | Graph (HashMap-based) |
| Search User (prefix) | O(k) | Trie (k=key length) |
| Like Post | O(1) avg | Set (Hash-based) |
| Get Recommendations | O(n) | Graph traversal |
| Send Message | O(1) | Queue + HashMap |

### Space Complexity

| Component | Complexity | Notes |
|-----------|-----------|-------|
| User Storage | O(n) | n = number of users |
| Post Storage | O(p) | p = number of posts |
| Relationship Graph | O(n + e) | e = number of edges |
| Trie (Search) | O(n × k) | k = avg username length |
| Message Queue | O(m) | m = number of messages |

## Scalability Considerations

### Current Limitations
1. **In-memory storage**: Limited by RAM
2. **Single-threaded**: No concurrent operations
3. **File I/O**: Slow for large datasets
4. **Linear search**: Some operations iterate all data

### Potential Improvements
1. **Database integration**: PostgreSQL, MongoDB
2. **Caching layer**: Redis for hot data
3. **Indexing**: B-trees for range queries
4. **Sharding**: Distribute data across servers
5. **Load balancing**: Handle multiple concurrent users
6. **Async I/O**: Non-blocking file operations

## Security Architecture

### Current Implementation
- Plain text password storage (⚠️ NOT SECURE)
- Basic input validation
- No SQL injection (no SQL used)
- No XSS protection (no web interface)

### Recommended Improvements
1. **Password hashing**: bcrypt, argon2
2. **Salt**: Unique per user
3. **Rate limiting**: Prevent brute force
4. **Input sanitization**: Prevent injection attacks
5. **Session management**: Secure tokens
6. **HTTPS**: Encrypted communication (for web API)
7. **Authentication tokens**: JWT for stateless auth
8. **Authorization**: Role-based access control

## Extension Points

### Easy to Add
- New notification types
- Additional search algorithms
- More recommendation strategies
- Custom feed filters

### Moderate Effort
- Comments on posts
- Hashtag system
- Group/community features
- Media upload support

### Major Refactoring Required
- Real-time messaging (WebSocket)
- Database migration
- RESTful API layer
- Web/mobile frontend
- Microservices architecture

## Testing Strategy

### Current Status
- No automated tests
- Manual testing via CLI

### Recommended Testing Approach

1. **Unit Tests**:
   - Test each data structure independently
   - Test each manager class in isolation
   - Mock dependencies

2. **Integration Tests**:
   - Test workflows (login, create post, etc.)
   - Test cross-module interactions
   - Verify data consistency

3. **System Tests**:
   - End-to-end user scenarios
   - Load testing with many users
   - Stress testing with large data

4. **Test Framework**: Google Test (gtest) recommended

## Deployment Considerations

### Development
```bash
make debug
./build/Mini_Instagram
```

### Production (Conceptual)
1. Build with optimizations: `make release`
2. Run with production data directory
3. Monitor logs for errors
4. Backup data files regularly
5. Implement log rotation

### Docker Deployment (Future)
```dockerfile
FROM ubuntu:latest
RUN apt-get update && apt-get install -y g++ make
COPY . /app
WORKDIR /app/SMP_backend
RUN make release
CMD ["./build/Mini_Instagram"]
```

## Conclusion

This architecture provides a solid foundation for a social media backend while demonstrating key data structures and algorithms. The modular design allows for easy extension and maintenance, making it suitable for educational purposes and potential real-world adaptation.

For production use, significant enhancements in security, scalability, and reliability would be required.
