// create entity and player class that extend entity
#include <iostream>

class Entity {
    private:
        int m_X, m_Y;
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

class Player : public Entity {
    public:
        Player() {
            std::cout << "Player created!" << std::endl;
        }

        ~Player() {
            std::cout << "Player destroyed!" << std::endl;
        }
};

int main() {
    std::cout << sizeof(Player) << std::endl; // Size of player class
    Player player;
    player.Print();
    return 0;
}