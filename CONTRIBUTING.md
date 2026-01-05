# Contributing to Mini Instagram

First off, thank you for considering contributing to Mini Instagram! It's people like you that make this project a great learning resource for data structures and algorithms.

## Code of Conduct

This project and everyone participating in it is expected to maintain a respectful and harassment-free environment. By participating, you are expected to uphold this code.

## How Can I Contribute?

### Reporting Bugs

Before creating bug reports, please check the existing issues to avoid duplicates. When you create a bug report, include as many details as possible:

- **Use a clear and descriptive title**
- **Describe the exact steps to reproduce the problem**
- **Provide specific examples** to demonstrate the steps
- **Describe the behavior you observed** and what you expected
- **Include screenshots** if applicable
- **Specify your environment**: OS, compiler version, etc.

### Suggesting Enhancements

Enhancement suggestions are tracked as GitHub issues. When creating an enhancement suggestion:

- **Use a clear and descriptive title**
- **Provide a detailed description** of the suggested enhancement
- **Explain why this enhancement would be useful**
- **List any similar features** in other projects if applicable

### Pull Requests

1. **Fork the repository** and create your branch from `main`
2. **Follow the coding style** used throughout the project
3. **Write clear commit messages**
4. **Include comments** in your code where necessary
5. **Test your changes** thoroughly
6. **Update documentation** if you're changing functionality

## Development Process

### Setting Up Development Environment

```bash
# Clone the repository
git clone https://github.com/Furqan-2006/DS-Project-2025.git
cd DS-Project-2025/SMP_backend

# Build the project
make debug

# Run the application
./build/Mini_Instagram
```

### Coding Standards

#### C++ Style Guide

- **Naming Conventions**:
  - Classes: `PascalCase` (e.g., `SystemManager`)
  - Functions: `camelCase` (e.g., `getUserProfile`)
  - Variables: `camelCase` (e.g., `currentUserID`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_USERS`)
  - Private members: `camelCase` (e.g., `userMgr`)

- **Code Formatting**:
  - Indentation: 4 spaces (no tabs)
  - Line length: Maximum 100 characters
  - Braces: Opening brace on same line for functions/classes
  - Comments: Use `//` for single-line, `/* */` for multi-line

- **Header Files**:
  - Use `#pragma once` for include guards
  - Order includes: standard library, then project headers
  - Forward declare when possible to reduce dependencies

- **Best Practices**:
  - Use `const` wherever appropriate
  - Prefer references over pointers when not nullable
  - Initialize all member variables
  - Use smart pointers when ownership is complex
  - Avoid raw `new`/`delete` when possible
  - Check for null pointers before dereferencing

#### Example Code Style

```cpp
#pragma once

#include <string>
#include <vector>
#include "core/user.hpp"

class ExampleClass
{
private:
    int memberVariable;
    std::string userName;

public:
    ExampleClass() : memberVariable(0), userName("") {}
    
    // Brief description of what this function does
    bool performAction(const std::string& input)
    {
        if (input.empty())
        {
            return false;
        }
        
        // Implementation details
        userName = input;
        return true;
    }
    
    int getMemberVariable() const { return memberVariable; }
};
```

### Building and Testing

#### Building

```bash
# Release build
make release

# Debug build with symbols
make debug

# Clean build artifacts
make clean
```

#### Testing

Before submitting a pull request:

1. **Build the project** without warnings
2. **Test core functionality**:
   - User registration and login
   - Post creation and viewing
   - Friend requests
   - Messaging system
   - Search functionality
3. **Test edge cases**:
   - Empty inputs
   - Very long inputs
   - Non-existent users/posts
   - Duplicate actions
4. **Check for memory leaks** (if possible):
   ```bash
   make memcheck
   ```

### Commit Message Guidelines

- Use the present tense ("Add feature" not "Added feature")
- Use the imperative mood ("Move cursor to..." not "Moves cursor to...")
- Limit the first line to 72 characters or less
- Reference issues and pull requests after the first line

Good commit message examples:
```
Add user profile validation

Fix memory leak in PostManager destructor

Update README with build instructions

Refs #123 - Implement notification system
```

## Project Structure

Understanding the project structure will help you contribute effectively:

```
SMP_backend/
├── include/          # Header files
│   ├── ADT/         # Custom data structures
│   ├── core/        # Core business logic
│   ├── content/     # Content management
│   ├── interaction/ # User interactions
│   ├── system/      # System-level modules
│   └── utils/       # Utility functions
├── src/             # Implementation files (mirrors include/)
├── data/            # Data storage
└── build/           # Compiled binaries
```

## Areas for Contribution

We especially welcome contributions in these areas:

### High Priority
- **Security**: Password hashing, input sanitization
- **Testing**: Unit tests, integration tests
- **Documentation**: Code comments, user guides
- **Bug Fixes**: Any reported issues

### Medium Priority
- **Performance**: Optimization of algorithms
- **Features**: Comments on posts, hashtags, groups
- **UI/UX**: Better CLI interface, web frontend
- **Data Persistence**: Database integration

### Low Priority (Future Enhancements)
- **Media Support**: Image/video handling
- **Analytics**: User statistics, trending algorithms
- **Localization**: Multi-language support
- **API**: RESTful API development

## Questions?

Feel free to open an issue with the tag `question` if you have any questions about contributing.

## Recognition

Contributors will be acknowledged in:
- The project README
- Release notes
- Special contributors file (if applicable)

Thank you for contributing to Mini Instagram! 🎉
