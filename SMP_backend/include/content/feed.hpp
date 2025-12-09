#pragma once

#include "post.hpp"
#include "core/relationGraph.hpp"
#include <vector>

class FeedManager
{
private:
    PostManager *pm;
    RelationshipGraph *rg;

    // Helper: Heapify for max-heap (newest first)
    void heapifyDown(std::vector<Post *> &heap, size_t index, size_t heapSize) const;
    void buildMaxHeap(std::vector<Post *> &posts) const;

    // Helper: Extract top K from heap
    std::vector<Post *> extractTopK(std::vector<Post *> &heap, size_t k) const;

public:
    FeedManager(PostManager *p, RelationshipGraph *r);

    // Helper: Merge K sorted lists (each user's posts are sorted newest first)
    std::vector<Post *> mergeKSortedLists(const std::vector<std::vector<Post *>> &lists, size_t limit) const;
    // Generate feed from users you follow (using heap and merge algorithms)
    std::vector<Post *> getUserFeed(ull userID, size_t limit = 50) const;

    // Feed from mutual friends (bidirectional follows)
    std::vector<Post *> getUserFeedByFriends(ull userID, size_t limit = 50) const;

    // Feed from your own posts
    std::vector<Post *> getUserPersonalFeed(ull userID, size_t limit = 50) const;

    void displayFeed(ull userID, size_t limit = 50) const;
};