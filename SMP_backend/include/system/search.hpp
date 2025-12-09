#pragma once
#include "core/user.hpp"
#include "content/post.hpp"
#include "ADT/hash_map.hpp"
#include "ADT/trie.hpp"
#include <vector>

enum class SearchType
{
    USER,
    SYSTEM
};

enum class SearchMode
{
    EXACT,
    PARTIAL,
    PREFIX
};

class SearchEngine
{
private:
    HashMap<std::string, User *> userMap; // exact username lookup
    std::vector<User *> users;            // for partial/fuzzy search

    std::vector<Post *> systemItems; // all posts
    Trie<Post *> systemTrie;         // for prefix search on words in content

    // Helper: Uses KMP or Boyer-Moore based on text length
    bool containsSubstring(const std::string &text, const std::string &pattern);

public:
    SearchEngine();

    // Adding items
    void addUser(User *user);
    void addSystemItem(Post *item);

    // Main search interface
    std::vector<void *> search(const std::string &query, SearchType type, SearchMode mode);

    // Specialized searches
    std::vector<User *> searchUsers(const std::string &query, SearchMode mode);
    std::vector<Post *> searchSystem(const std::string &query, SearchMode mode);

    // Multi-pattern search (uses Rabin-Karp)
    std::vector<Post *> multiPatternSearchPosts(const std::vector<std::string> &queries);
};