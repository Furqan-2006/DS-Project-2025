#pragma once

#include "content/post.hpp"
#include "core/relationGraph.hpp"

class RecommendationEngine
{
private:
    PostManager *pm;
    RelationshipGraph *rg;

public:
    RecommendationEngine(PostManager *pm, RelationshipGraph *rg);

    std::vector<Post *> recommendPosts(ull userID, size_t limit = 20) const;
    std::vector<ull> recommendUsers(ull userID, size_t limit = 10) const;
    std::vector<Post *> recommendTrendingPosts(size_t limit = 20) const;

    void displayRecommendedPosts(ull userID, size_t limit = 20) const;
    void displayRecommendedUsers(ull userID, size_t limit = 20) const;
};