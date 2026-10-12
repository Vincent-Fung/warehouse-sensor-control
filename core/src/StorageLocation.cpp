#include "warehouse/StorageLocation.hpp"
#include "warehouse/Article.hpp"
#include <stdexcept>

using namespace std;

int StorageLocation::getId() const{
    return id;
}

bool StorageLocation::canFit(const Article& article, double quantity) const{


    if (!storedArticleId.has_value()) {
        return quantity <= capacity - occupiedCapacity;
    }

    if (storedArticleId.value() == article.getId()) {
        return quantity <= capacity - occupiedCapacity;
    }

    return false;
    

  
}

void StorageLocation::assign(const Article& article, double quantity){
    if(!canFit(article, quantity)){
        throw runtime_error("Not enough capacity");
    }
    occupiedCapacity += quantity;
    storedArticleId = article.getId();
}

void StorageLocation::release(double quantity){
    if(quantity > occupiedCapacity){
        throw runtime_error("Not enough occupied capacity");
    }
    occupiedCapacity -= quantity;
    if(occupiedCapacity == 0){
        storedArticleId.reset();
    }
}


std::optional<int> StorageLocation::getStoredArticleId() const
{
    return storedArticleId;
}

    
StorageLocation::StorageLocation(int id, double capacity, double occupiedCapacity, std::optional<int> storedArticleId)
    : id(id), capacity(capacity), occupiedCapacity(occupiedCapacity), storedArticleId(storedArticleId)
{
}

StorageLocation::~StorageLocation()
{
}






