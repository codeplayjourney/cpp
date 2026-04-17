#include <iostream>
#include <string>

class Entity {
private:
    int* m_X, *m_Y;
    mutable int debugCount; // This member can be modified even in const functions
public:
    const int* const GetX() const { return m_X; }
    int* GetY() const { return m_Y; }
    void IncrementDebugCount() const { ++debugCount; }
};

void PrintEntity(const Entity& e) {
    std::cout << *e.GetX() << ", " << *e.GetY() << std::endl;
    e.IncrementDebugCount(); // Allowed because debugCount is mutable
}

int main() {
    int x = 8;
    auto f = [=]() mutable {
        x++; // Allowed because the lambda is mutable
        std::cout << "Inside lambda: " << x << std::endl;
    };
    
    return 0;
}