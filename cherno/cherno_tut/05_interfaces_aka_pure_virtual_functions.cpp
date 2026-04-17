#include <iostream>
#include <string>

class Printable {
    public:
    virtual std::string GetClassName() = 0; // pure virtual function, makes Printable an abstract class
};

class Entity : public Printable {
    public:
    virtual std::string GetName() = 0; // pure virtual function, makes Entity an abstract class
    std::string GetClassName() override {
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
    Entity* e = new Player("Cherno");
    std::cout << e->GetName() << std::endl; // Output: Cherno
    std::cout << e->GetClassName() << std::endl; // Output: Entity
    Player* p = new Player("Cherno");
    std::cout << p->GetName() << std::endl; // Output: Cherno
    std::cout << p->GetClassName() << std::endl; // Output: Entity
    
    return 0;
}