#include "warehouse/Article.hpp"

using namespace std;



int Article::getId() const{
    return id;
}

string Article::getName() const{
    return name;
}

double Article::getRequiredCapacity() const{
        return requiredCapacity;
}
    
Article::Article(int id, const string& name, double requiredCapacity)
    : id(id), name(name), requiredCapacity(requiredCapacity)
{
}

Article::~Article()
{
}






