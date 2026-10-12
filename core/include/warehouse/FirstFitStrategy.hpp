#pragma once
#include "warehouse/PlacementStrategy.hpp"

class FirstFitStrategy : public PlacementStrategy {
public:
    bool place(std::vector<StorageLocation>& locations, const Article& article, double quantity) override;
};