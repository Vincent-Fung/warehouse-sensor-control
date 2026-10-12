#pragma once
#include <memory>
#include <optional>
#include <unordered_map>
#include <vector>
#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"
#include "warehouse/PlacementStrategy.hpp"

class Warehouse {
private:
    std::vector<StorageLocation> locations;
    std::unordered_map<int, Article> articles;
    std::unique_ptr<PlacementStrategy> m_strategy;

public:
    bool storeArticle(int articleId, double quantity);
    Warehouse(const std::vector<StorageLocation>& locations,
              const std::vector<Article>& articleList,
              std::unique_ptr<PlacementStrategy> strategy);
    ~Warehouse();
};