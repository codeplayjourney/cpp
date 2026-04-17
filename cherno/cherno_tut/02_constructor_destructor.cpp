// create entity class with constructor and destructor
#include <iostream>

class Entity {
    public:
        Entity() {
            std::cout << "Entity created!" << std::endl;
        }

        ~Entity() {
            std::cout << "Entity destroyed!" << std::endl;
        }

        void Print() {
            std::cout << "Hello World!" << std::endl;
        }
};

void Function() {
    Entity entity;
    entity.Print();
}
int main() {
    Function();
    return 0;
}