#pragma once
#include <string>


class Article {
private:
    int id;
    std::string name;
    double requiredCapacity;

public:
    int getId() const;
    std::string getName() const;
    double getRequiredCapacity() const;
    Article(int id, const std::string& name, double requiredCapacity);
    ~Article();
};