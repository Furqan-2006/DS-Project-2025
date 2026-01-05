# Mini Instagram - Social Media Platform Backend

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![C++](https://img.shields.io/badge/C++-17-blue.svg)](https://isocpp.org/)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)](https://github.com/Furqan-2006/DS-Project-2025)

A comprehensive social media platform backend built in C++ as a Data Structures course project. This project implements a fully functional social network system with custom data structures, demonstrating real-world applications of graphs, hash maps, tries, queues, and other advanced data structures.

## 🌟 Features

### Core Functionality
- **User Management**: Registration, login/logout, profile management
- **Social Connections**: Follow/unfollow users, friend requests, mutual friends
- **Content Creation**: Create, view, like, and delete posts
- **Feed System**: Personalized feed, friends-only feed, trending posts
- **Messaging**: Direct messaging between users with chat history
- **Notifications**: Real-time notification system for user activities
- **Search**: Prefix-based search for users and posts using Trie data structure
- **Recommendations**: User and post recommendation engine
- **Online Status**: Track and display user online/offline status

### Technical Highlights
- **Custom Data Structures**: All implemented from scratch
  - Hash Map with chaining
  - Graph (directed and undirected) for social relationships
  - Trie for efficient prefix searching
  - Linked Lists, Stacks, Queues, and Sets
- **System Architecture**: Modular design with clear separation of concerns
- **Data Persistence**: File-based storage system
- **CLI Interface**: Rich command-line interface with color-coded output

## 📁 Project Structure

```
DS-Project-2025/
├── SMP_backend/
│   ├── include/              # Header files
│   │   ├── ADT/             # Abstract Data Types (custom implementations)
│   │   │   ├── graph.hpp
│   │   │   ├── hash_map.hpp
│   │   │   ├── linked_list.hpp
│   │   │   ├── queue.hpp
│   │   │   ├── set.hpp
│   │   │   ├── stack.hpp
│   │   │   └── trie.hpp
│   │   ├── core/            # Core system modules
│   │   │   ├── followerList.hpp
│   │   │   ├── relationGraph.hpp
│   │   │   ├── status.hpp
│   │   │   └── user.hpp
│   │   ├── content/         # Content management
│   │   │   ├── feed.hpp
│   │   │   ├── post.hpp
│   │   │   └── recommendation.hpp
│   │   ├── interaction/     # User interactions
│   │   │   ├── friendRequest.hpp
│   │   │   └── message.hpp
│   │   ├── system/          # System-level modules
│   │   │   ├── notification.hpp
│   │   │   ├── search.hpp
│   │   │   └── SystemManager.hpp
│   │   └── utils/           # Utility functions
│   │       ├── fileIO.hpp
│   │       ├── helpers.hpp
│   │       └── validation.hpp
│   ├── src/                 # Implementation files
│   │   ├── core/
│   │   ├── content/
│   │   ├── interaction/
│   │   ├── system/
│   │   ├── utils/
│   │   └── main.cpp
│   ├── data/                # Data storage
│   │   └── logs/
│   └── build/               # Compiled binaries
├── README.md
├── LICENSE
└── .gitignore
```

## 🚀 Getting Started

### Prerequisites
- C++ compiler with C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- Make (optional, for Makefile build)
- CMake 3.10+ (optional, for CMake build)

### Building the Project

#### Option 1: Using Make (Recommended)
```bash
cd SMP_backend
make
./build/Mini_Instagram
```

#### Option 2: Using CMake
```bash
mkdir build && cd build
cmake ../SMP_backend
make
./Mini_Instagram
```

#### Option 3: Manual Compilation
```bash
cd SMP_backend
g++ -std=c++17 -O2 -Wall -Wextra \
    src/main.cpp \
    src/content/feed.cpp \
    src/content/post.cpp \
    src/content/recommendation.cpp \
    src/core/followerList.cpp \
    src/core/relationGraph.cpp \
    src/core/user.cpp \
    src/core/status.cpp \
    src/interaction/message.cpp \
    src/interaction/friendRequest.cpp \
    src/system/notification.cpp \
    src/system/search.cpp \
    src/system/systemManager.cpp \
    src/utils/fileIO.cpp \
    src/utils/helpers.cpp \
    src/utils/validation.cpp \
    -I include \
    -o build/Mini_Instagram
```

### Running the Application
```bash
cd SMP_backend
./build/Mini_Instagram
```

## 💡 Usage

### Main Menu
Upon launching, you'll see the main menu:
1. **Register New Account** - Create a new user account
2. **Login** - Sign in with existing credentials
3. **View All Users** - Browse registered users
4. **System Status** - View system statistics
0. **Exit** - Close the application

### User Dashboard (After Login)
- **My Profile** - View and edit your profile
- **Create Post** - Share new content
- **View Feed** - See posts from followed users
- **My Posts** - Manage your posts
- **Search Users/Posts** - Find users and content
- **Notifications** - Check your notifications
- **Messages** - Send and receive direct messages
- **Friend Requests** - Manage connection requests
- **Manage Connections** - View followers, following, and friends
- **Recommendations** - Discover new users and content
- **Settings** - Update profile, change password, delete account

## 🏗️ Architecture

### System Manager
The `SystemManager` class acts as the central orchestrator, coordinating between:
- **User Manager**: Handles authentication and user profiles
- **Relationship Graph**: Manages follow/friend connections and likes
- **Status Manager**: Tracks online/offline status
- **Post Manager**: Handles post creation, retrieval, and deletion
- **Feed Manager**: Generates personalized feeds using ranking algorithms
- **Recommendation Engine**: Suggests users and posts based on social graph
- **Search Engine**: Implements prefix-based searching using Trie
- **Message System**: Manages direct messages with queue-based delivery
- **Notification Manager**: Handles user notifications
- **Friend Request Manager**: Processes friend request workflows

### Data Structures Used

| Data Structure | Use Case | Implementation |
|---------------|----------|----------------|
| **Graph** | Social relationships (follow/friend) | Custom adjacency list |
| **Hash Map** | User lookup, post storage | Custom chaining implementation |
| **Trie** | Prefix search for users/posts | Custom implementation |
| **Queue** | Message system, notifications | Custom linked list based |
| **Set** | Like tracking, unique collections | Custom hash-based |
| **Linked List** | General-purpose collections | Custom singly linked |
| **Stack** | Undo operations (future use) | Custom array-based |

## 🔧 Configuration

The application stores data in the `data/` directory:
- `user.txt` - User credentials and profiles
- `logs/` - System logs and debugging information

## 🧪 Testing

To test the application:
1. Build and run the application
2. Register multiple test accounts
3. Test various features (posts, messages, friend requests)
4. Verify data persistence by restarting the application

## 📈 Performance Characteristics

| Operation | Time Complexity | Space Complexity |
|-----------|----------------|------------------|
| User Login | O(1) average | O(n) |
| Create Post | O(1) | O(n) |
| Get Feed | O(f × p) where f=friends, p=posts | O(p) |
| Search User (prefix) | O(k) where k=key length | O(n × k) |
| Follow User | O(1) average | O(e) where e=edges |
| Get Recommendations | O(n) | O(n) |

## 🚧 Future Enhancements

- [ ] Add password hashing (bcrypt/argon2)
- [ ] Implement post comments and replies
- [ ] Add media support (images, videos)
- [ ] Implement hashtag system
- [ ] Add group/community features
- [ ] Real-time messaging with WebSocket
- [ ] Database integration (SQLite/PostgreSQL)
- [ ] RESTful API layer
- [ ] Web frontend interface
- [ ] Mobile app support
- [ ] Advanced analytics and insights
- [ ] Content moderation system
- [ ] Multi-language support

## 🤝 Contributing

Contributions are welcome! Please follow these guidelines:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

Please ensure your code follows the existing style and includes appropriate documentation.

## 📝 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- **Furqan-2006** - *Initial work and development*

## 🙏 Acknowledgments

- Built as part of the Data Structures course project 2025
- Inspired by real-world social media platforms
- Thanks to all contributors and reviewers

## 📞 Contact

For questions, suggestions, or issues, please open an issue on GitHub or contact the maintainers.

---

**Note**: This is an educational project demonstrating data structures and algorithms. It is not intended for production use without significant security enhancements and proper database integration.
