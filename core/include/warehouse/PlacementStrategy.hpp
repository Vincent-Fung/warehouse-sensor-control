#pragma once
#include <vector>
#include "warehouse/Article.hpp"
#include "warehouse/StorageLocation.hpp"

class PlacementStrategy {
public:
    virtual ~PlacementStrategy() = default;
    virtual bool place(std::vector<StorageLocation>& locations, const Article& article, double quantity) = 0;
};