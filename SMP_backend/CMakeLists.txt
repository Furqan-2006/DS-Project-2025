cmake_minimum_required(VERSION 3.10)
project(MiniInstagram VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Compiler flags
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/content/feed.cpp
    src/content/post.cpp
    src/content/recommendation.cpp
    src/core/followerList.cpp
    src/core/relationGraph.cpp
    src/core/user.cpp
    src/core/status.cpp
    src/interaction/message.cpp
    src/interaction/friendRequest.cpp
    src/system/notification.cpp
    src/system/search.cpp
    src/system/systemManager.cpp
    src/utils/fileIO.cpp
    src/utils/helpers.cpp
    src/utils/validation.cpp
)

# Header files (for IDE support)
set(HEADERS
    include/main.hpp
    include/ADT/graph.hpp
    include/ADT/hash_map.hpp
    include/ADT/linked_list.hpp
    include/ADT/queue.hpp
    include/ADT/set.hpp
    include/ADT/stack.hpp
    include/ADT/trie.hpp
    include/core/followerList.hpp
    include/core/relationGraph.hpp
    include/core/status.hpp
    include/core/user.hpp
    include/content/feed.hpp
    include/content/post.hpp
    include/content/recommendation.hpp
    include/interaction/friendRequest.hpp
    include/interaction/message.hpp
    include/system/notification.hpp
    include/system/search.hpp
    include/system/SystemManager.hpp
    include/utils/fileIO.hpp
    include/utils/helpers.hpp
    include/utils/validation.hpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Set executable properties
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME "Mini_Instagram"
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/build
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(${PROJECT_NAME} PRIVATE _WIN32)
elseif(UNIX)
    target_compile_definitions(${PROJECT_NAME} PRIVATE UNIX)
endif()

# Build type specific flags
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Building in Debug mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -g -O0)
    endif()
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Building in Release mode")
    target_compile_definitions(${PROJECT_NAME} PRIVATE NDEBUG)
    if(NOT MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE -O2)
    endif()
endif()

# Create data directory
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data)
file(MAKE_DIRECTORY ${CMAKE_SOURCE_DIR}/data/logs)

# Print build information
message(STATUS "")
message(STATUS "Mini Instagram - Social Media Platform")
message(STATUS "========================================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "Output directory: ${CMAKE_SOURCE_DIR}/build")
message(STATUS "")

# Installation rules (optional)
install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/data
    DESTINATION share/${PROJECT_NAME}
    OPTIONAL
)

# Testing support (for future use)
option(BUILD_TESTING "Build tests" OFF)
if(BUILD_TESTING)
    enable_testing()
    # Add test subdirectory when tests are created
    # add_subdirectory(tests)
endif()

# Documentation generation (optional)
option(BUILD_DOCS "Build documentation" OFF)
if(BUILD_DOCS)
    find_package(Doxygen)
    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/docs)
        doxygen_add_docs(docs
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
            COMMENT "Generating API documentation with Doxygen"
        )
    else()
        message(WARNING "Doxygen not found, documentation will not be built")
    endif()
endif()
