#include "content/post.hpp"
#include <iostream>
#include <cstring>

ull Post::nextpID = 1000;

// ============================================================================
// Post Implementation
// ============================================================================

Post::Post(ull id, ull author, const std::string &text)
    : postID(id), authorID(author), content(text)
{
    timestamp = std::time(nullptr);
}

ull Post::getPostID() const { return postID; }
ull Post::getAuthor() const { return authorID; }
const std::string &Post::getContent() const { return content; }
std::time_t Post::getTimestamp() const { return timestamp; }
size_t Post::getLikesCount() const { return likes.size(); }

void Post::setContent(const std::string &newContent) { content = newContent; }

void Post::addLikes(ull userID)
{
    for (ull id : likes)
        if (id == userID)
            return;
    likes.push_back(userID);
}

void Post::removeLike(ull userID)
{
    for (size_t i = 0; i < likes.size(); i++)
    {
        if (likes[i] == userID)
        {
            likes[i] = likes.back();
            likes.pop_back();
            return;
        }
    }
}

json Post::toJSON() const
{
    json j;
    j["postID"] = postID;
    j["authorID"] = authorID;
    j["content"] = content;
    j["timestamp"] = timestamp;
    j["likes"] = likes;
    return j;
}

Post Post::fromJSON(const json &j)
{
    Post p(
        j["postID"].get<ull>(),
        j["authorID"].get<ull>(),
        j["content"].get<std::string>());

    p.timestamp = j["timestamp"].get<std::time_t>();
    p.likes = j["likes"].get<std::vector<ull>>();

    return p;
}

void Post::display() const
{
    std::cout << "Post ID: " << postID
              << " | Author ID: " << authorID
              << " | Time: " << std::ctime(&timestamp)
              << "Content: " << content
              << " | Likes: " << getLikesCount() << "\n";
}

// ============================================================================
// PostList Implementation (Linked List)
// ============================================================================

PostList::PostList() : head(nullptr), count(0) {}

PostList::~PostList()
{
    clear();
}

void PostList::addPost(Post *p)
{
    // Add to front for newest-first ordering
    PostNode *newNode = new PostNode(p);
    newNode->next = head;
    head = newNode;
    count++;
}

bool PostList::removePost(ull postID)
{
    PostNode *curr = head;
    PostNode *prev = nullptr;

    while (curr)
    {
        if (curr->post->getPostID() == postID)
        {
            if (prev)
                prev->next = curr->next;
            else
                head = curr->next;

            delete curr;
            count--;
            return true;
        }
        prev = curr;
        curr = curr->next;
    }
    return false;
}

Post *PostList::findPost(ull postID) const
{
    PostNode *curr = head;
    while (curr)
    {
        if (curr->post->getPostID() == postID)
            return curr->post;
        curr = curr->next;
    }
    return nullptr;
}

std::vector<Post *> PostList::getAllPosts() const
{
    std::vector<Post *> result;
    PostNode *curr = head;

    while (curr)
    {
        result.push_back(curr->post);
        curr = curr->next;
    }

    return result;
}

size_t PostList::size() const
{
    return count;
}

void PostList::clear()
{
    PostNode *curr = head;
    while (curr)
    {
        PostNode *next = curr->next;
        delete curr;
        curr = next;
    }
    head = nullptr;
    count = 0;
}

// ============================================================================
// KMP String Matching for Post Search
// ============================================================================

namespace PostSearch
{
    std::vector<int> buildLPS(const std::string &pattern)
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
        std::vector<int> lps = buildLPS(pattern);

        int i = 0;
        int j = 0;

        while (i < n)
        {
            if (text[i] == pattern[j])
            {
                i++;
                j++;
            }

            if (j == m)
                return true;
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
// PostManager Implementation
// ============================================================================

PostManager::PostManager(const std::string &path)
    : nextPostID(1000), filePath(path)
{
    loadFromFile();
}

PostManager::~PostManager()
{
    // Clean up all posts
    for (auto it = postIndex.begin(); it != postIndex.end(); ++it)
    {
        delete (*it).value;
    }

    // Clean up all PostLists
    for (auto it = userPosts.begin(); it != userPosts.end(); ++it)
    {
        delete (*it).value;
    }
}

Post *PostManager::createPost(ull authorID, const std::string &content)
{
    Post *p = new Post(nextPostID++, authorID, content);

    // Add to post index
    postIndex.insert(p->getPostID(), p);

    // Add to user's post list
    PostList **userList = userPosts.get(authorID);
    if (!userList)
    {
        PostList *newList = new PostList();
        userPosts.insert(authorID, newList);
        userList = userPosts.get(authorID);
    }

    (*userList)->addPost(p);

    return p;
}

bool PostManager::deletePost(ull postID)
{
    Post **p = postIndex.get(postID);
    if (!p)
        return false;

    ull authorID = (*p)->getAuthor();

    // Remove from user's list
    PostList **userList = userPosts.get(authorID);
    if (userList)
    {
        (*userList)->removePost(postID);
    }

    // Remove from index
    delete *p;
    postIndex.remove(postID);

    return true;
}

bool PostManager::editPost(ull postID, const std::string &newContent)
{
    if (!canEdit(postID))
        return false;

    Post **p = postIndex.get(postID);
    if (!p)
        return false;

    (*p)->setContent(newContent);
    return true;
}

bool PostManager::canEdit(ull postID) const
{
    Post *const *p = postIndex.get(postID);
    if (!p)
        return false;

    std::time_t now = std::time(nullptr);
    std::time_t postTime = (*p)->getTimestamp();

    // Check if within edit window
    return (now - postTime) <= EDIT_WINDOW;
}

Post *PostManager::getPost(ull postID) const
{
    Post *const *p = postIndex.get(postID);
    return p ? *p : nullptr;
}

std::vector<Post *> PostManager::getPostsByUser(ull userID) const
{
    PostList *const *userList = userPosts.get(userID);

    if (!userList)
        return {};

    // Already sorted newest first due to linked list insertion at head
    return (*userList)->getAllPosts();
}

std::vector<Post *> PostManager::getAllPosts() const
{
    std::vector<Post *> result;

    for (auto it = postIndex.begin(); it != postIndex.end(); ++it)
    {
        result.push_back((*it).value);
    }

    // Sort by timestamp (newest first)
    // Manual quicksort implementation
    if (result.size() > 1)
    {
        auto sortFunc = [](std::vector<Post *> &arr, int low, int high, auto &self) -> void
        {
            if (low < high)
            {
                Post *pivot = arr[high];
                int i = low - 1;

                for (int j = low; j < high; j++)
                {
                    if (arr[j]->getTimestamp() > pivot->getTimestamp())
                    {
                        i++;
                        Post *temp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = temp;
                    }
                }

                Post *temp = arr[i + 1];
                arr[i + 1] = arr[high];
                arr[high] = temp;

                int pi = i + 1;
                self(arr, low, pi - 1, self);
                self(arr, pi + 1, high, self);
            }
        };

        sortFunc(result, 0, result.size() - 1, sortFunc);
    }

    return result;
}

std::vector<Post *> PostManager::searchPosts(const std::string &keyword) const
{
    std::vector<Post *> results;

    if (keyword.empty())
        return results;

    std::string lowerKeyword = PostSearch::toLower(keyword);

    for (auto it = postIndex.begin(); it != postIndex.end(); ++it)
    {
        Post *p = (*it).value;
        std::string lowerContent = PostSearch::toLower(p->getContent());

        if (PostSearch::kmpSearch(lowerContent, lowerKeyword))
        {
            results.push_back(p);
        }
    }

    return results;
}

std::vector<Post *> PostManager::searchPostsByUser(ull userID, const std::string &keyword) const
{
    std::vector<Post *> results;

    if (keyword.empty())
        return results;

    std::string lowerKeyword = PostSearch::toLower(keyword);

    PostList *const *userList = userPosts.get(userID);
    if (!userList)
        return results;

    std::vector<Post *> userPostsVec = (*userList)->getAllPosts();

    for (Post *p : userPostsVec)
    {
        std::string lowerContent = PostSearch::toLower(p->getContent());

        if (PostSearch::kmpSearch(lowerContent, lowerKeyword))
        {
            results.push_back(p);
        }
    }

    return results;
}

bool PostManager::saveToFile() const
{
    json j;
    j["nextID"] = nextPostID;
    j["posts"] = json::array();

    for (auto it = postIndex.begin(); it != postIndex.end(); ++it)
    {
        j["posts"].push_back((*it).value->toJSON());
    }

    std::ofstream out(filePath);
    if (!out)
        return false;

    out << j.dump(4);
    out.close();
    return true;
}

bool PostManager::loadFromFile()
{
    std::ifstream in(filePath);
    if (!in)
        return false;

    json j;
    in >> j;
    in.close();

    nextPostID = j["nextID"].get<ull>();

    for (auto &pj : j["posts"])
    {
        Post temp = Post::fromJSON(pj);
        Post *p = new Post(temp);

        ull authorID = p->getAuthor();

        // Add to index
        postIndex.insert(p->getPostID(), p);

        // Add to user's list
        PostList **userList = userPosts.get(authorID);
        if (!userList)
        {
            PostList *newList = new PostList();
            userPosts.insert(authorID, newList);
            userList = userPosts.get(authorID);
        }

        (*userList)->addPost(p);
    }

    return true;
}

void PostManager::displayAll() const
{
    for (auto it = postIndex.begin(); it != postIndex.end(); ++it)
    {
        (*it).value->display();
    }
}