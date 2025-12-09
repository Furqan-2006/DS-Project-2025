#include "content/feed.hpp"
#include <iostream>

FeedManager::FeedManager(PostManager *p, RelationshipGraph *r)
    : pm(p), rg(r) {}

// ============================================================================
// Max-Heap Operations (for newest-first ordering by timestamp)
// ============================================================================

void FeedManager::heapifyDown(std::vector<Post *> &heap, size_t index, size_t heapSize) const
{
    size_t largest = index;
    size_t left = 2 * index + 1;
    size_t right = 2 * index + 2;

    // Max-heap: parent's timestamp > children's timestamps (newer posts first)
    if (left < heapSize && heap[left]->getTimestamp() > heap[largest]->getTimestamp())
        largest = left;

    if (right < heapSize && heap[right]->getTimestamp() > heap[largest]->getTimestamp())
        largest = right;

    if (largest != index)
    {
        Post *temp = heap[index];
        heap[index] = heap[largest];
        heap[largest] = temp;

        heapifyDown(heap, largest, heapSize);
    }
}

void FeedManager::buildMaxHeap(std::vector<Post *> &posts) const
{
    // Build max-heap in O(n) time
    int n = posts.size();

    // Start from last non-leaf node and heapify down
    for (int i = n / 2 - 1; i >= 0; i--)
    {
        heapifyDown(posts, i, n);
    }
}

std::vector<Post *> FeedManager::extractTopK(std::vector<Post *> &heap, size_t k) const
{
    std::vector<Post *> result;
    size_t heapSize = heap.size();

    // Extract top K elements from max-heap
    for (size_t i = 0; i < k && heapSize > 0; i++)
    {
        // Root is the maximum
        result.push_back(heap[0]);

        // Move last element to root
        heap[0] = heap[heapSize - 1];
        heapSize--;

        // Heapify down from root
        if (heapSize > 0)
            heapifyDown(heap, 0, heapSize);
    }

    return result;
}

// ============================================================================
// Merge K Sorted Lists (each user's posts are already sorted newest first)
// ============================================================================

std::vector<Post *> FeedManager::mergeKSortedLists(const std::vector<std::vector<Post *>> &lists, size_t limit) const
{
    std::vector<Post *> result;

    if (lists.empty())
        return result;

    // Min-heap structure for merge algorithm
    // Each element: {post, listIndex, postIndex}
    struct HeapNode
    {
        Post *post;
        size_t listIdx;
        size_t postIdx;
    };

    std::vector<HeapNode> minHeap;

    // Helper: Heapify down for min-heap (by timestamp, newest first = larger timestamp)
    auto heapifyDownMinHeap = [&](size_t index, size_t heapSize)
    {
        while (true)
        {
            size_t largest = index;
            size_t left = 2 * index + 1;
            size_t right = 2 * index + 2;

            // For newest-first: we want LARGER timestamps at top
            if (left < heapSize &&
                minHeap[left].post->getTimestamp() > minHeap[largest].post->getTimestamp())
                largest = left;

            if (right < heapSize &&
                minHeap[right].post->getTimestamp() > minHeap[largest].post->getTimestamp())
                largest = right;

            if (largest == index)
                break;

            HeapNode temp = minHeap[index];
            minHeap[index] = minHeap[largest];
            minHeap[largest] = temp;

            index = largest;
        }
    };

    // Initialize heap with first post from each list
    for (size_t i = 0; i < lists.size(); i++)
    {
        if (!lists[i].empty())
        {
            minHeap.push_back({lists[i][0], i, 0});
        }
    }

    // Build initial heap
    for (int i = (int)minHeap.size() / 2 - 1; i >= 0; i--)
    {
        heapifyDownMinHeap(i, minHeap.size());
    }

    // Extract posts in sorted order (newest first)
    while (!minHeap.empty() && result.size() < limit)
    {
        // Extract max (root of max-heap)
        HeapNode top = minHeap[0];
        result.push_back(top.post);

        // Get next post from same list
        size_t nextIdx = top.postIdx + 1;
        if (nextIdx < lists[top.listIdx].size())
        {
            minHeap[0] = {lists[top.listIdx][nextIdx], top.listIdx, nextIdx};
            heapifyDownMinHeap(0, minHeap.size());
        }
        else
        {
            // No more posts in this list, remove from heap
            minHeap[0] = minHeap.back();
            minHeap.pop_back();
            if (!minHeap.empty())
                heapifyDownMinHeap(0, minHeap.size());
        }
    }

    return result;
}

// ============================================================================
// Feed Generation
// ============================================================================

std::vector<Post *> FeedManager::getUserFeed(ull userID, size_t limit) const
{
    // Get all users this user follows
    const Set<NodeID> *following = rg->getFollowing(userID);

    if (!following || following->size() == 0)
        return {};

    // Collect posts from all followed users (K sorted lists)
    std::vector<std::vector<Post *>> allUserPosts;

    const auto &followingData = following->data();
    for (size_t i = 0; i < following->size(); i++)
    {
        ull followedUserID = followingData[i];
        std::vector<Post *> userPosts = pm->getPostsByUser(followedUserID);

        if (!userPosts.empty())
            allUserPosts.push_back(userPosts);
    }

    // Add user's own posts
    std::vector<Post *> ownPosts = pm->getPostsByUser(userID);
    if (!ownPosts.empty())
        allUserPosts.push_back(ownPosts);

    // Merge K sorted lists (each user's posts are sorted newest first)
    return mergeKSortedLists(allUserPosts, limit);
}

std::vector<Post *> FeedManager::getUserFeedByFriends(ull userID, size_t limit) const
{
    // Get mutual friends (bidirectional follows)
    std::vector<NodeID> friends = rg->getFriends(userID);

    if (friends.empty())
        return {};

    // Collect posts from all friends
    std::vector<std::vector<Post *>> allFriendPosts;

    for (size_t i = 0; i < friends.size(); i++)
    {
        std::vector<Post *> friendPosts = pm->getPostsByUser(friends[i]);

        if (!friendPosts.empty())
            allFriendPosts.push_back(friendPosts);
    }

    // Add user's own posts
    std::vector<Post *> ownPosts = pm->getPostsByUser(userID);
    if (!ownPosts.empty())
        allFriendPosts.push_back(ownPosts);

    // Merge K sorted lists
    return mergeKSortedLists(allFriendPosts, limit);
}

std::vector<Post *> FeedManager::getUserPersonalFeed(ull userID, size_t limit) const
{
    // Just return user's own posts (already sorted newest first)
    std::vector<Post *> posts = pm->getPostsByUser(userID);

    if (posts.size() > limit)
        posts.resize(limit);

    return posts;
}

void FeedManager::displayFeed(ull userID, size_t limit) const
{
    std::vector<Post *> feed = getUserFeed(userID, limit);

    std::cout << "=== Feed for User " << userID << " ===" << std::endl;
    std::cout << "Showing " << feed.size() << " posts" << std::endl
              << std::endl;

    for (size_t i = 0; i < feed.size(); i++)
    {
        std::cout << "[" << (i + 1) << "] ";
        feed[i]->display();
        std::cout << std::endl;
    }
}