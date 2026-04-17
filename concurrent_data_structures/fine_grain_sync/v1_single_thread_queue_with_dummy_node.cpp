#include <iostream>
#include <mutex>
#include <thread>

template<typename T>
class SingleThreadQueueWithDummyNode {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T value) : data(value), next(nullptr) {} // member initializer list syntax
    };
    std::unique_ptr<Node> front;
    Node* back;
public:
    SingleThreadQueueWithDummyNode() : front(std::make_unique<Node>(T())), back(front.get()) {}
    // pop: we pop the first node and shift front to point to first node->next
    // push(new data): we change data to be the new data and make next point to new dummy node
    void push(T value) {
        back->data = std::move(value);
        std::unique_ptr<Node> newDummyNode = std::make_unique<Node>(T());
        back->next = std::move(newDummyNode);
        // point back to the new dummy node
        back = back->next.get();
    }

    std::shared_ptr<T> try_pop() {
        if (front.get() == back) {
            // check if queue only contains dummy node
            return std::shared_ptr<T>(); // return empty shared_ptr if queue is empty
        }
        // dont auto suggest let me try first
        std::shared_ptr<T> res = std::make_shared<T>(std::move(front->data));
        // shift front to point to the next node
        front = std::move(front->next);
        return res;
    }
};

int main() {
    SingleThreadQueueWithDummyNode<int> queue;
    // single threaded usage of the queue can be demonstrated here. Still not yet thread safe.
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