#include <iostream>
#include <string>

class Entity {
private:
    std::string m_Name;
    int m_Age;
public:
    Entity(const std::string &name) : m_Name(name), m_Age(0) {}
    Entity(int age) : m_Name("Unknown"), m_Age(age) {}
};

void PrintEntity(const Entity &entity) {
    // print entity details
}

int main() {
    Entity a("cherno");
    Entity b = 22; // implicit conversion, will call the constructor that takes an int
    PrintEntity(22); // implicit conversion, will call the constructor that takes an int
    PrintEntity(Entity(22)); // explicit conversion, will call the constructor that takes an int
    // PrintEntity("Cherno"); // doesnt compile, because there is no constructor that takes a const char*, and we can only do implicit conversions at most once (in this case, two is needed as we need to convert const char* to std::string, and then std::string to Entity)
    return 0;
}