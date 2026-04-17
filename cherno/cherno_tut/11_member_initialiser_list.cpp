#include <iostream>
#include <string>

class Example {
private:
    int m_X;
public:
    Example(int x) : m_X(x) {}
};

class Entity {
private:
    std::string m_Name;
    Example m_Example;

public:
    Entity()
        : m_Name("Default Name"), m_Example(42) // Member initializer list
    {
    }
    // same as:
    // Entity() {
    //     m_Name = "Default Name"; // Assignment, not initialization
    //     m_Example = Example(42); // Assignment, not initialization
    // }
};
int main() {
    Entity e;
    return 0;
}