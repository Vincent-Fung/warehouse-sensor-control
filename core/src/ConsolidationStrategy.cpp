#include "warehouse/ConsolidationStrategy.hpp"

using namespace std;

bool ConsolidationStrategy::place(vector<StorageLocation>& locations, const Article& article, double quantity) {
    int firstFitIndex = -1;

    for (size_t i = 0; i < locations.size(); i++) {
        if (!locations[i].canFit(article, quantity)) {
            continue;
        }

        if (firstFitIndex == -1) {
            firstFitIndex = static_cast<int>(i);
        }

        auto stored = locations[i].getStoredArticleId();
        if (stored.has_value() && stored.value() == article.getId()) {
            locations[i].assign(article, quantity);
            return true;
        }
    }

    if (firstFitIndex != -1) {
        locations[firstFitIndex].assign(article, quantity);
        return true;
    }

    return false;
}