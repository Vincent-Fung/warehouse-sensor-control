#include "warehouse/FirstFitStrategy.hpp"

using namespace std;

bool FirstFitStrategy::place(vector<StorageLocation>& locations, const Article& article, double quantity) {
    for (size_t i = 0; i < locations.size(); i++) {
        if (locations[i].canFit(article, quantity)) {
            locations[i].assign(article, quantity);
            return true;
        }
    }
    return false;
}