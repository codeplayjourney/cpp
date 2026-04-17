#include <iostream>
#include <string>

class Entity {
    public:
    virtual std::string GetName() {
        return "Entity";
    }
};

class Player : public Entity {
    private:
        std::string m_Name;
    public:
    Player(const std::string& name) : m_Name(name) {}
    std::string GetName() override {
        return m_Name;
    }
};

int main() {
    Entity* e = new Entity();
    std::cout << e->GetName() << std::endl; // Output: Entity
    Player* p = new Player("Cherno");
    std::cout << p->GetName() << std::endl; // Output: Cherno
    Entity* e2 = p; // Upcasting
    std::cout << e2->GetName() << std::endl; // Output: Entity (because GetName is not virtual)
    return 0;
}