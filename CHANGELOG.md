# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Comprehensive documentation suite including:
  - README.md with project overview and usage instructions
  - ARCHITECTURE.md with system design details
  - CONTRIBUTING.md with contribution guidelines
  - SECURITY.md with security considerations and recommendations
  - IMPROVEMENTS.md with scaling roadmap and enhancement ideas
  - DEVELOPER_GUIDE.md with development workflows and best practices
- Build system improvements:
  - Makefile for easier compilation
  - CMakeLists.txt for cross-platform build support
  - setup.sh script for quick project setup
  - .clang-format for consistent code formatting
- Testing infrastructure:
  - Test directory structure
  - Example unit tests
  - Testing guide and documentation
- Configuration system:
  - config.json template with application settings
- Project files:
  - .gitignore to exclude build artifacts
  - MIT LICENSE
  - CHANGELOG.md (this file)

### Changed
- Fixed compiler warnings in validation.cpp (unused variables)
- Updated .gitignore to allow custom Makefile

### Security
- Documented security vulnerabilities and recommendations in SECURITY.md
- Added password hashing implementation examples
- Included input sanitization guidelines

## [1.0.0] - 2025-01-05

### Added
- Initial release of Mini Instagram social media platform backend
- Core features:
  - User registration and authentication
  - User profiles with city information
  - Follow/unfollow system
  - Friend request system
  - Post creation and deletion
  - Post likes/unlikes
  - Personalized feed generation
  - Direct messaging between users
  - Notification system
  - User and post search (prefix-based)
  - Recommendation engine for users and posts
  - Online/offline status tracking
  
- Custom data structures:
  - HashMap with chaining
  - Directed Graph for social relationships
  - Trie for efficient prefix searching
  - Queue for message delivery
  - Set for unique collections
  - Linked List for general collections
  - Stack for future features
  
- Core modules:
  - SystemManager - Central orchestration
  - UserManager - User management
  - PostManager - Post management
  - RelationshipGraph - Social graph
  - FeedManager - Feed generation
  - RecommendationEngine - Content recommendations
  - SearchEngine - Search functionality
  - MessageSystem - Messaging
  - NotificationManager - Notifications
  - StatusManager - Online status
  - FriendRequestManager - Friend requests
  
- CLI interface:
  - Color-coded output
  - Menu-driven navigation
  - User dashboard with statistics
  
- Data persistence:
  - File-based storage system
  - Auto-save functionality

### Technical Specifications
- Language: C++17
- Build System: G++ compiler
- Architecture: Modular design with clear separation of concerns
- Code Size: ~34,000 lines of code

---

## Version History

### Version Numbering
We use [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for new functionality in a backward-compatible manner
- PATCH version for backward-compatible bug fixes

### Categories
- **Added** - New features
- **Changed** - Changes in existing functionality
- **Deprecated** - Soon-to-be removed features
- **Removed** - Removed features
- **Fixed** - Bug fixes
- **Security** - Security fixes and improvements

---

## Future Releases (Planned)

### [1.1.0] - Planned
- [ ] Unit test suite with Google Test
- [ ] Integration tests for workflows
- [ ] Code coverage reporting
- [ ] Password hashing implementation
- [ ] Input sanitization improvements
- [ ] Configuration file support
- [ ] Enhanced logging system

### [1.2.0] - Planned
- [ ] Comment system for posts
- [ ] Hashtag support
- [ ] Media upload capability (images)
- [ ] Group/community features
- [ ] Enhanced search filters
- [ ] User blocking functionality

### [2.0.0] - Planned (Breaking Changes)
- [ ] Database integration (SQLite/PostgreSQL)
- [ ] RESTful API layer
- [ ] Authentication with JWT tokens
- [ ] WebSocket for real-time messaging
- [ ] Microservices architecture
- [ ] Docker containerization

---

## Links
- [Repository](https://github.com/Furqan-2006/DS-Project-2025)
- [Issues](https://github.com/Furqan-2006/DS-Project-2025/issues)
- [Pull Requests](https://github.com/Furqan-2006/DS-Project-2025/pulls)
