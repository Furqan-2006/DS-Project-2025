#include "system/search.hpp"
#include <iostream>

// ============================================================================
// String Matching Algorithms
// ============================================================================

namespace StringSearch
{
    // -------------------------------------------------------------------------
    // KMP Algorithm - O(n + m) - Best for general purpose, stable
    // -------------------------------------------------------------------------
    std::vector<int> buildKMPTable(const std::string &pattern)
    {
        int m = pattern.length();
        std::vector<int> lps(m, 0);
        int len = 0;
        int i = 1;

        while (i < m)
        {
            if (pattern[i] == pattern[len])
            {
                len++;
                lps[i] = len;
                i++;
            }
            else
            {
                if (len != 0)
                    len = lps[len - 1];
                else
                {
                    lps[i] = 0;
                    i++;
                }
            }
        }
        return lps;
    }

    bool kmpSearch(const std::string &text, const std::string &pattern)
    {
        if (pattern.empty())
            return true;
        if (text.empty() || text.length() < pattern.length())
            return false;

        int n = text.length();
        int m = pattern.length();
        std::vector<int> lps = buildKMPTable(pattern);

        int i = 0; // text index
        int j = 0; // pattern index

        while (i < n)
        {
            if (text[i] == pattern[j])
            {
                i++;
                j++;
            }

            if (j == m)
                return true; // Match found
            else if (i < n && text[i] != pattern[j])
            {
                if (j != 0)
                    j = lps[j - 1];
                else
                    i++;
            }
        }
        return false;
    }

    // -------------------------------------------------------------------------
    // Boyer-Moore Algorithm - O(n/m) best case - Best for long texts
    // -------------------------------------------------------------------------
    void buildBadCharTable(const std::string &pattern, int badChar[256])
    {
        int m = pattern.length();

        // Initialize all occurrences as -1
        for (int i = 0; i < 256; i++)
            badChar[i] = -1;

        // Fill actual value of last occurrence
        for (int i = 0; i < m; i++)
            badChar[(unsigned char)pattern[i]] = i;
    }

    bool boyerMooreSearch(const std::string &text, const std::string &pattern)
    {
        if (pattern.empty())
            return true;
        if (text.empty() || text.length() < pattern.length())
            return false;

        int n = text.length();
        int m = pattern.length();

        int badChar[256];
        buildBadCharTable(pattern, badChar);

        int shift = 0; // shift of pattern with respect to text

        while (shift <= (n - m))
        {
            int j = m - 1;

            // Keep reducing j while characters match
            while (j >= 0 && pattern[j] == text[shift + j])
                j--;

            if (j < 0)
            {
                // Pattern found
                return true;
            }
            else
            {
                // Shift pattern so that bad character aligns
                // with last occurrence in pattern
                int badCharShift = j - badChar[(unsigned char)text[shift + j]];
                shift += (badCharShift > 1) ? badCharShift : 1;
            }
        }
        return false;
    }

    // -------------------------------------------------------------------------
    // Rabin-Karp Algorithm - O(n + m) average - Best for multiple patterns
    // -------------------------------------------------------------------------
    const int PRIME = 101; // Prime number for hashing

    long long computeHash(const std::string &str, int start, int len)
    {
        long long hash = 0;
        for (int i = 0; i < len; i++)
        {
            hash = hash * 256 + (unsigned char)str[start + i];
            hash %= PRIME;
        }
        return hash;
    }

    long long recomputeHash(const std::string &str, int oldIndex, int newIndex,
                            long long oldHash, int patternLen)
    {
        long long newHash = oldHash;

        // Remove leading character
        long long pow256 = 1;
        for (int i = 0; i < patternLen - 1; i++)
            pow256 = (pow256 * 256) % PRIME;

        newHash = (newHash - (unsigned char)str[oldIndex] * pow256) % PRIME;
        if (newHash < 0)
            newHash += PRIME;

        // Add trailing character
        newHash = (newHash * 256 + (unsigned char)str[newIndex]) % PRIME;

        return newHash;
    }

    bool rabinKarpSearch(const std::string &text, const std::string &pattern)
    {
        if (pattern.empty())
            return true;
        if (text.empty() || text.length() < pattern.length())
            return false;

        int n = text.length();
        int m = pattern.length();

        long long patternHash = computeHash(pattern, 0, m);
        long long textHash = computeHash(text, 0, m);

        for (int i = 0; i <= n - m; i++)
        {
            // Check hash match
            if (patternHash == textHash)
            {
                // Verify actual string match (avoid hash collision)
                bool match = true;
                for (int j = 0; j < m; j++)
                {
                    if (text[i + j] != pattern[j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                    return true;
            }

            // Compute hash for next window
            if (i < n - m)
            {
                textHash = recomputeHash(text, i, i + m, textHash, m);
            }
        }
        return false;
    }

    // -------------------------------------------------------------------------
    // Multi-Pattern Rabin-Karp - Search for multiple patterns simultaneously
    // -------------------------------------------------------------------------
    std::vector<bool> multiPatternSearch(const std::string &text,
                                         const std::vector<std::string> &patterns)
    {
        std::vector<bool> results(patterns.size(), false);

        if (text.empty() || patterns.empty())
            return results;

        // For each pattern length, group patterns and search
        HashMap<int, std::vector<size_t>> patternsByLength;

        for (size_t i = 0; i < patterns.size(); i++)
        {
            int len = patterns[i].length();
            std::vector<size_t> *indices = patternsByLength.get(len);
            if (!indices)
            {
                std::vector<size_t> newVec;
                newVec.push_back(i);
                patternsByLength.insert(len, newVec);
            }
            else
            {
                indices->push_back(i);
            }
        }

        // Search for patterns of each length
        for (auto it = patternsByLength.begin(); it != patternsByLength.end(); ++it)
        {
            int m = (*it).key;
            const std::vector<size_t> &indices = (*it).value;

            if (m > (int)text.length())
                continue;

            // Compute hashes for all patterns of this length
            std::vector<long long> patternHashes;
            for (size_t idx : indices)
            {
                patternHashes.push_back(computeHash(patterns[idx], 0, m));
            }

            // Slide window through text
            long long textHash = computeHash(text, 0, m);

            for (int i = 0; i <= (int)text.length() - m; i++)
            {
                // Check against all patterns of this length
                for (size_t j = 0; j < indices.size(); j++)
                {
                    size_t patternIdx = indices[j];

                    if (textHash == patternHashes[j])
                    {
                        // Verify match
                        bool match = true;
                        for (int k = 0; k < m; k++)
                        {
                            if (text[i + k] != patterns[patternIdx][k])
                            {
                                match = false;
                                break;
                            }
                        }
                        if (match)
                            results[patternIdx] = true;
                    }
                }

                // Recompute hash for next window
                if (i < (int)text.length() - m)
                {
                    textHash = recomputeHash(text, i, i + m, textHash, m);
                }
            }
        }

        return results;
    }

    // -------------------------------------------------------------------------
    // Case-insensitive conversion
    // -------------------------------------------------------------------------
    std::string toLower(const std::string &str)
    {
        std::string result = str;
        for (char &c : result)
        {
            if (c >= 'A' && c <= 'Z')
                c = c + ('a' - 'A');
        }
        return result;
    }
}

// ============================================================================
// SearchEngine Implementation
// ============================================================================

SearchEngine::SearchEngine() {}

void SearchEngine::addUser(User *user)
{
    userMap.insert(user->getUname(), user);
    users.push_back(user);
}

void SearchEngine::addSystemItem(Post *item)
{
    systemItems.push_back(item);

    // For prefix search, tokenize content into words
    std::string content = item->getContent();
    std::string word;

    for (size_t i = 0; i <= content.length(); i++)
    {
        if (i == content.length() || content[i] == ' ' || content[i] == '\n')
        {
            if (!word.empty())
            {
                std::string lowerWord = StringSearch::toLower(word);
                systemTrie.insert(lowerWord, &item);
                word.clear();
            }
        }
        else
        {
            word += content[i];
        }
    }
}

// ============================================================================
// Helper: Choose best algorithm based on text length
// ============================================================================
bool SearchEngine::containsSubstring(const std::string &text, const std::string &pattern)
{
    if (pattern.empty())
        return true;
    if (text.empty())
        return false;

    std::string lowerText = StringSearch::toLower(text);
    std::string lowerPattern = StringSearch::toLower(pattern);

    // Choose algorithm based on text/pattern characteristics
    if (lowerText.length() > 1000)
    {
        // Use Boyer-Moore for long texts (better average case)
        return StringSearch::boyerMooreSearch(lowerText, lowerPattern);
    }
    else
    {
        // Use KMP for general purpose (stable, predictable)
        return StringSearch::kmpSearch(lowerText, lowerPattern);
    }
}

// ============================================================================
// User Search
// ============================================================================
std::vector<User *> SearchEngine::searchUsers(const std::string &query, SearchMode mode)
{
    std::vector<User *> results;

    if (mode == SearchMode::EXACT)
    {
        User **uPtr = userMap.get(query);
        if (uPtr && *uPtr)
            results.push_back(*uPtr);
    }
    else if (mode == SearchMode::PARTIAL)
    {
        // Use KMP for partial username matching
        std::string lowerQuery = StringSearch::toLower(query);

        for (size_t i = 0; i < users.size(); ++i)
        {
            std::string lowerUsername = StringSearch::toLower(users[i]->getUname());

            if (StringSearch::kmpSearch(lowerUsername, lowerQuery))
                results.push_back(users[i]);
        }
    }
    else if (mode == SearchMode::PREFIX)
    {
        // Prefix search on usernames
        std::string lowerQuery = StringSearch::toLower(query);

        for (size_t i = 0; i < users.size(); ++i)
        {
            std::string lowerUsername = StringSearch::toLower(users[i]->getUname());

            if (lowerUsername.length() >= lowerQuery.length())
            {
                bool match = true;
                for (size_t j = 0; j < lowerQuery.length(); j++)
                {
                    if (lowerUsername[j] != lowerQuery[j])
                    {
                        match = false;
                        break;
                    }
                }
                if (match)
                    results.push_back(users[i]);
            }
        }
    }

    return results;
}

// ============================================================================
// System Search (Posts)
// ============================================================================
std::vector<Post *> SearchEngine::searchSystem(const std::string &query, SearchMode mode)
{
    std::vector<Post *> results;

    if (mode == SearchMode::EXACT)
    {
        std::string lowerQuery = StringSearch::toLower(query);

        for (size_t i = 0; i < systemItems.size(); ++i)
        {
            std::string lowerContent = StringSearch::toLower(systemItems[i]->getContent());

            if (lowerContent == lowerQuery)
                results.push_back(systemItems[i]);
        }
    }
    else if (mode == SearchMode::PARTIAL)
    {
        // Use smart algorithm selection based on content length
        for (size_t i = 0; i < systemItems.size(); ++i)
        {
            if (containsSubstring(systemItems[i]->getContent(), query))
                results.push_back(systemItems[i]);
        }
    }
    else if (mode == SearchMode::PREFIX)
    {
        // Search for posts containing words with this prefix
        std::string lowerQuery = StringSearch::toLower(query);
        auto foundPosts = systemTrie.searchPrefix(lowerQuery);

        if (!foundPosts.empty())
        {
            // Remove duplicates
            HashMap<ull, bool> seen;

            for (size_t i = 0; i < foundPosts.size(); i++)
            {
                Post *post = *foundPosts[i]; // Dereference because it's Post**
                if (!seen.contains(post->getPostID()))
                {
                    results.push_back(post);
                    seen.insert(post->getPostID(), true);
                }
            }
        }
    }

    return results;
}

// ============================================================================
// Multi-Pattern Search (Bonus feature using Rabin-Karp)
// ============================================================================
std::vector<Post *> SearchEngine::multiPatternSearchPosts(const std::vector<std::string> &queries)
{
    HashMap<ull, int> postMatchCount; // postID -> number of pattern matches

    for (size_t i = 0; i < systemItems.size(); i++)
    {
        Post *post = systemItems[i];
        std::string content = StringSearch::toLower(post->getContent());

        // Convert queries to lowercase
        std::vector<std::string> lowerQueries;
        for (const auto &q : queries)
        {
            lowerQueries.push_back(StringSearch::toLower(q));
        }

        // Use multi-pattern Rabin-Karp
        std::vector<bool> matches = StringSearch::multiPatternSearch(content, lowerQueries);

        // Count matches
        int matchCount = 0;
        for (bool match : matches)
        {
            if (match)
                matchCount++;
        }

        if (matchCount > 0)
        {
            postMatchCount.insert(post->getPostID(), matchCount);
        }
    }

    // Return posts sorted by match count (most relevant first)
    std::vector<Post *> results;

    for (size_t i = 0; i < systemItems.size(); i++)
    {
        if (postMatchCount.contains(systemItems[i]->getPostID()))
        {
            results.push_back(systemItems[i]);
        }
    }

    return results;
}

// ============================================================================
// Main Search Interface
// ============================================================================
std::vector<void *> SearchEngine::search(const std::string &query, SearchType type, SearchMode mode)
{
    std::vector<void *> results;

    if (type == SearchType::USER)
    {
        std::vector<User *> usersFound = searchUsers(query, mode);
        for (size_t i = 0; i < usersFound.size(); ++i)
            results.push_back((void *)usersFound[i]);
    }
    else // SYSTEM
    {
        std::vector<Post *> postsFound = searchSystem(query, mode);
        for (size_t i = 0; i < postsFound.size(); ++i)
            results.push_back((void *)postsFound[i]);
    }

    return results;
}