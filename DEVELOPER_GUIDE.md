# Developer Guide

## Getting Started

### Prerequisites
- C++ compiler with C++17 support (GCC 7+, Clang 5+, MSVC 2017+)
- Make or CMake (optional but recommended)
- Git for version control

### Quick Start

1. **Clone the repository**
   ```bash
   git clone https://github.com/Furqan-2006/DS-Project-2025.git
   cd DS-Project-2025
   ```

2. **Build the project**
   ```bash
   ./setup.sh
   ```

3. **Run the application**
   ```bash
   ./setup.sh run
   ```

## Project Organization

### Directory Structure
```
DS-Project-2025/
├── SMP_backend/           # Main application code
│   ├── include/          # Header files (.hpp)
│   ├── src/              # Implementation files (.cpp)
│   ├── data/             # Application data
│   ├── build/            # Compiled binaries (gitignored)
│   ├── Makefile          # Build configuration
│   └── CMakeLists.txt    # CMake configuration
├── tests/                # Test suite
├── docs/                 # Additional documentation
├── README.md             # Project overview
├── ARCHITECTURE.md       # System architecture
├── CONTRIBUTING.md       # Contribution guidelines
├── SECURITY.md          # Security considerations
├── IMPROVEMENTS.md      # Roadmap and scaling guide
└── setup.sh             # Quick setup script
```

## Build System

### Using Makefile (Recommended)

```bash
cd SMP_backend

# Build release version
make release

# Build debug version with symbols
make debug

# Clean build artifacts
make clean

# Run the application
make run

# Check for memory leaks (requires valgrind)
make memcheck

# View available commands
make help
```

### Using CMake

```bash
mkdir build && cd build
cmake ../SMP_backend -DCMAKE_BUILD_TYPE=Release
make
./Mini_Instagram
```

### Manual Compilation

```bash
cd SMP_backend
g++ -std=c++17 -O2 -Wall -Wextra \
    src/**/*.cpp \
    -I include \
    -o build/Mini_Instagram
```

## Development Workflow

### 1. Create a Feature Branch
```bash
git checkout -b feature/my-new-feature
```

### 2. Make Your Changes
- Follow the coding style (see Coding Standards below)
- Write tests for new functionality
- Update documentation as needed

### 3. Build and Test
```bash
make clean
make debug
# Test your changes manually
./build/Mini_Instagram
```

### 4. Commit Your Changes
```bash
git add .
git commit -m "Add: Description of your changes"
```

### 5. Push and Create Pull Request
```bash
git push origin feature/my-new-feature
# Create PR on GitHub
```

## Coding Standards

### Style Guidelines

#### Naming Conventions
- **Classes**: `PascalCase` - `SystemManager`, `UserManager`
- **Functions**: `camelCase` - `getUserById`, `createPost`
- **Variables**: `camelCase` - `currentUser`, `postCount`
- **Constants**: `UPPER_SNAKE_CASE` - `MAX_USERS`, `DEFAULT_LIMIT`
- **File names**: `lowercase_with_underscores` - `system_manager.cpp`

#### Code Formatting
- **Indentation**: 4 spaces (no tabs)
- **Line length**: Maximum 100 characters
- **Braces**: K&R style for functions/classes

```cpp
// Good
void functionName()
{
    if (condition)
    {
        doSomething();
    }
}

// Use clang-format to auto-format
clang-format -i src/file.cpp
```

#### Comments
```cpp
// Single-line comments for brief explanations
int count = 0;  // Track number of users

/*
 * Multi-line comments for detailed explanations
 * Use for complex algorithms or important notes
 */

/**
 * @brief Documentation comment for functions
 * @param userID The user identifier
 * @return Pointer to User object or nullptr
 */
User* getUser(ull userID);
```

### Best Practices

#### Memory Management
```cpp
// Good: Use RAII and smart pointers
std::unique_ptr<User> user = std::make_unique<User>();

// Acceptable for this project: Manual management with clear ownership
User* user = new User();
// ... use user
delete user;  // Always clean up

// Avoid: Raw pointers without clear ownership
User* user = getUserFromSomewhere();  // Who owns this?
```

#### Error Handling
```cpp
// Check for null pointers
User* user = getUser(userID);
if (!user)
{
    std::cerr << "Error: User not found\n";
    return false;
}

// Validate inputs
bool validateUsername(const std::string& username)
{
    if (username.empty() || username.length() > MAX_USERNAME_LENGTH)
    {
        return false;
    }
    return true;
}
```

#### Const Correctness
```cpp
// Use const for read-only parameters
void displayUser(const User& user);

// Use const for methods that don't modify state
int getUserCount() const;

// Use const for variables that won't change
const int MAX_ATTEMPTS = 5;
```

## Module Overview

### Core Modules

#### SystemManager
- **Purpose**: Central orchestrator for all modules
- **Location**: `include/system/SystemManager.hpp`
- **Key Methods**:
  - `initializeAllModules()`
  - `performCompleteLogin()`
  - `performCreatePostComplete()`

#### UserManager
- **Purpose**: User authentication and profile management
- **Location**: `include/core/user.hpp`
- **Responsibilities**: User CRUD, authentication, profile updates

#### RelationshipGraph
- **Purpose**: Social graph management
- **Location**: `include/core/relationGraph.hpp`
- **Responsibilities**: Follow/friend relationships, mutual friends

#### PostManager
- **Purpose**: Post creation and management
- **Location**: `include/content/post.hpp`
- **Responsibilities**: Post CRUD, like tracking

### Data Structures

All custom data structures are in `include/ADT/`:
- `hash_map.hpp` - Hash table with chaining
- `graph.hpp` - Directed graph for relationships
- `trie.hpp` - Prefix tree for search
- `queue.hpp` - FIFO queue for messages
- `set.hpp` - Hash-based set
- `stack.hpp` - LIFO stack
- `linked_list.hpp` - Singly linked list

## Debugging

### Using GDB
```bash
# Build with debug symbols
make debug

# Run with GDB
gdb ./build/Mini_Instagram

# Common GDB commands
(gdb) break main           # Set breakpoint
(gdb) run                  # Start program
(gdb) next                 # Step over
(gdb) step                 # Step into
(gdb) print variable       # Print value
(gdb) backtrace           # Show call stack
```

### Memory Leak Detection
```bash
# Build debug version
make debug

# Run with valgrind
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./build/Mini_Instagram
```

### Common Issues

**Issue**: Segmentation fault
- **Cause**: Null pointer dereference, array out of bounds
- **Debug**: Use gdb or valgrind to find the exact line

**Issue**: Build fails with linker errors
- **Cause**: Missing implementation, multiple definitions
- **Fix**: Check that all `.cpp` files are included in Makefile

**Issue**: Program crashes on exit
- **Cause**: Double delete, memory corruption
- **Debug**: Run with valgrind to detect memory errors

## Performance Optimization

### Profiling
```bash
# Build with profiling enabled
g++ -pg -std=c++17 src/*.cpp -I include -o program

# Run program
./program

# Generate profile report
gprof program gmon.out > analysis.txt
```

### Optimization Tips
1. **Use references**: Pass large objects by const reference
2. **Reserve capacity**: For vectors/hashmaps if size is known
3. **Avoid copies**: Use move semantics where appropriate
4. **Cache results**: Store frequently accessed data
5. **Profile first**: Don't optimize without measuring

## Adding New Features

### Example: Adding a "Like Comment" Feature

1. **Update Data Structure**
```cpp
// include/content/comment.hpp
class Comment {
private:
    Set<ull> likedBy;  // Add this
    
public:
    bool addLike(ull userID);
    bool removeLike(ull userID);
    int getLikeCount() const;
};
```

2. **Implement Methods**
```cpp
// src/content/comment.cpp
bool Comment::addLike(ull userID) {
    return likedBy.insert(userID);
}

bool Comment::removeLike(ull userID) {
    return likedBy.erase(userID);
}

int Comment::getLikeCount() const {
    return likedBy.size();
}
```

3. **Update Manager**
```cpp
// include/system/SystemManager.hpp
bool likeComment(ull userID, ull commentID);
bool unlikeComment(ull userID, ull commentID);
```

4. **Add to CLI**
```cpp
// src/main.cpp - in appropriate menu
int choice = getChoice("1. Like\n2. Unlike\nChoice: ", 1, 2);
if (choice == 1) {
    sysMgr->likeComment(userID, commentID);
}
```

5. **Write Tests**
```cpp
// tests/unit/test_comment.cpp
TEST(CommentTest, LikeComment) {
    Comment comment(1, 123, "Test comment");
    EXPECT_TRUE(comment.addLike(456));
    EXPECT_EQ(comment.getLikeCount(), 1);
}
```

## Useful Commands

### Code Analysis
```bash
# Find TODO items
grep -r "TODO" src/ include/

# Count lines of code
find src include -name "*.cpp" -o -name "*.hpp" | xargs wc -l

# Find memory leaks
valgrind --leak-check=full ./build/Mini_Instagram

# Check code style
clang-format --dry-run -Werror src/**/*.cpp
```

### Git Workflow
```bash
# Create feature branch
git checkout -b feature/my-feature

# Stage changes
git add src/modified_file.cpp

# Commit with message
git commit -m "Add: Feature description"

# Push to remote
git push origin feature/my-feature

# Update from main
git checkout main
git pull origin main
git checkout feature/my-feature
git merge main
```

## Resources

### Learning C++
- [cppreference.com](https://en.cppreference.com/) - C++ reference
- [learncpp.com](https://www.learncpp.com/) - C++ tutorials
- [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/)

### Data Structures
- Introduction to Algorithms (CLRS)
- Data Structures and Algorithm Analysis in C++ (Weiss)

### Tools
- [GDB Documentation](https://www.gnu.org/software/gdb/documentation/)
- [Valgrind Manual](https://valgrind.org/docs/manual/manual.html)
- [CMake Tutorial](https://cmake.org/cmake/help/latest/guide/tutorial/)

## Getting Help

- **Issues**: Open an issue on GitHub
- **Discussions**: Use GitHub Discussions
- **Documentation**: Check ARCHITECTURE.md and inline comments
- **Community**: Ask in project discussions

## Next Steps

1. Read [ARCHITECTURE.md](ARCHITECTURE.md) to understand the system design
2. Review [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines
3. Check [IMPROVEMENTS.md](IMPROVEMENTS.md) for feature ideas
4. Review [SECURITY.md](SECURITY.md) for security considerations

Happy coding! 🚀
