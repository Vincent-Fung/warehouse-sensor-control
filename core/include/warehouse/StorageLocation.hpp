#pragma once
#include <string>
#include <optional>
#include "warehouse/Article.hpp"


class StorageLocation {
private:
        int id;
        double capacity;
        double occupiedCapacity;
        std::optional<int> storedArticleId;


public:
    int getId() const;
    bool canFit(const Article& article, double quantity) const;
    void assign(const Article& article, double quantity); 
    void release(double quantity);
    std::optional<int> getStoredArticleId() const;
    StorageLocation(int id, double capacity ,double occupiedCapacity,std::optional<int> storedArticleId );
    ~StorageLocation();
};