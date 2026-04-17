#include <iostream>
#include <mutex>
#include <thread>

template<typename T>
class SingleThreadQueue {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T value) : data(value), next(nullptr) {} // member initializer list syntax
    };
    std::unique_ptr<Node> front;
    Node* back;
public:
    SingleThreadQueue() : front(nullptr), back(nullptr) {}
    void push(T value) {
        std::unique_ptr<Node> newNode = std::make_unique<Node>(std::move(value));
        if (back) {
            // queue is not empty, add new node to the end
            back->next = std::move(newNode);
            back = back->next.get();
        } else {
            // queue is empty, new node becomes both front and back
            front = std::move(newNode);
            back = front.get();
        }
    }

    std::shared_ptr<T> try_pop() {
        if (!front) {
            return std::shared_ptr<T>(); // return empty shared_ptr if queue is empty
        }
        
        std::shared_ptr<T> res = std::make_shared<T>(std::move(front->data));
        // shift front to point to the next node
        front = std::move(front->next);
        if (!front) {
            back = nullptr; // queue is now empty
        }
        return res;
    }
};

int main() {
    SingleThreadQueue<int> queue;
    // single threaded usage of the queue can be demonstrated here
    std::cout << "Pushing values 1, 2, 3 into the queue." << std::endl;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    std::cout << "Popping values from the queue:" << std::endl;
    for (int i = 0; i < 4; ++i) {
        std::shared_ptr<int> value = queue.try_pop();
        if (value) {
            std::cout << "Popped value: " << *value << std::endl;
        } else {
            std::cout << "Queue is empty." << std::endl;
        }
    }
    
    return 0;
}