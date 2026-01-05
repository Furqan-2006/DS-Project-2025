# Testing Guide for Mini Instagram

## Overview

This directory contains tests for the Mini Instagram project. The project uses Google Test (gtest) framework for unit and integration testing.

## Setting Up Testing Environment

### Install Google Test

#### On Ubuntu/Debian:
```bash
sudo apt-get install libgtest-dev
cd /usr/src/gtest
sudo cmake .
sudo make
sudo cp lib/*.a /usr/lib
```

#### Or build from source:
```bash
git clone https://github.com/google/googletest.git
cd googletest
mkdir build && cd build
cmake ..
make
sudo make install
```

### Build Tests

```bash
cd tests
mkdir build
cmake ..
make
```

### Run Tests

```bash
# Run all tests
./build/run_tests

# Run specific test suite
./build/run_tests --gtest_filter=HashMapTest.*

# Run with verbose output
./build/run_tests --gtest_verbose
```

## Test Structure

```
tests/
├── CMakeLists.txt          # CMake configuration for tests
├── README.md               # This file
├── unit/                   # Unit tests
│   ├── test_hashmap.cpp
│   ├── test_graph.cpp
│   └── test_trie.cpp
├── integration/            # Integration tests
│   ├── test_user_workflows.cpp
│   └── test_post_workflows.cpp
└── fixtures/               # Test data and fixtures
    └── test_data.hpp
```

## Example Test

See the example test files in the `unit/` and `integration/` directories for reference implementations.

## Best Practices

1. **Test One Thing**: Each test should verify one specific behavior
2. **Use Descriptive Names**: Test names should describe what they test
3. **Independent Tests**: Tests should not depend on each other
4. **Fast Tests**: Keep unit tests fast (<1ms each)

## Resources

- [Google Test Documentation](https://google.github.io/googletest/)
- [Google Test Primer](https://google.github.io/googletest/primer.html)
