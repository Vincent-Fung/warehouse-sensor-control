#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"
#include "warehouse/Warehouse.hpp"

using namespace std;

Warehouse::Warehouse(const vector<StorageLocation>& locations,
                      const vector<Article>& articleList,
                      unique_ptr<PlacementStrategy> strategy)
    : locations(locations), m_strategy(std::move(strategy))
{
    for (const auto& article : articleList) {
        articles.emplace(article.getId(), article);
    }
}

bool Warehouse::storeArticle(int articleId, double quantity) {
    auto it = articles.find(articleId);
    if (it == articles.end()) {
        return false;
    }
    return m_strategy->place(locations, it->second, quantity);
}

Warehouse::~Warehouse() {}