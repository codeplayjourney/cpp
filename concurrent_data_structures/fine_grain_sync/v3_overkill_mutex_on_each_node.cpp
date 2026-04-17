#include <iostream>
#include <thread>
#include <mutex>

template<typename T>
class MultithreadedQueueWithDummyNode {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::unique_ptr<Node> next;
        Node(T value) : data(std::make_shared<T>(std::move(value))), next(nullptr) {} // member initializer list syntax
        Node() : data(nullptr), next(nullptr) {}           // for dummy node, just in case T is not default-constructible
        std::mutex node_mutex; // mutex to protect access to the node
    };
    std::unique_ptr<Node> front;
    Node* back;
    std::mutex back_mutex; // mutex to protect access to the queue
    std::mutex front_mutex; // mutex to protect access to the queue

public:
    MultithreadedQueueWithDummyNode() : front(std::make_unique<Node>()), back(front.get()) {}
    // pop: we pop the first node and shift front to point to first node->next
    // push(new data): we change data to be the new data and make next point to new dummy node
    void push(T value) {
        std::shared_ptr<T> newData = std::make_shared<T>(std::move(value)); // shift this before locking mutex
        std::unique_ptr<Node> newDummyNode = std::make_unique<Node>();
        std::lock_guard<std::mutex> lock(back_mutex); // lock the mutex to protect access to the queue
        std::lock_guard<std::mutex> nodeLock(back->node_mutex); // lock the node mutex to protect access to the node
        back->data = std::move(newData);
        back->next = std::move(newDummyNode);
        // point back to the new dummy node
        back = back->next.get();
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> frontLock(front_mutex); // lock the mutex to protect access to the queue
        std::shared_ptr<T> res; // declared out here
        {
            std::lock_guard<std::mutex> nodeLock(front->node_mutex); // lock the node mutex to check if queue is empty
                // check if queue is empty
                if (front->next == nullptr) {
                    return std::shared_ptr<T>(); // return empty shared_ptr if queue is empty
                }
                res = std::move(front->data); // move the data out of the front node
        } // nodeLock releases here - node A's mutex unlocked while it's still alive
        // shift front to point to the next node
        front = std::move(front->next);
        return res;
    }
};


int main() {
    MultithreadedQueueWithDummyNode<int> queue;
    // create multiple producer and multiple consumer threads
    const int NUM_ITEMS_PER_PRODUCER = 20;
    const int NUM_PRODUCERS = 5;
    const int NUM_CONSUMERS = 5;
    std::thread producers[NUM_PRODUCERS];
    std::thread consumers[NUM_CONSUMERS];
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers[i] = std::thread([&queue, i]() {
            for (int j = 0; j < NUM_ITEMS_PER_PRODUCER; ++j) {
                queue.push(i * NUM_ITEMS_PER_PRODUCER + j);
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        });
    }
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers[i] = std::thread([&queue]() {
            for (int j = 0; j < NUM_ITEMS_PER_PRODUCER; ++j) {
                auto item = queue.try_pop();
                if (item) {
                    std::cout << "Consumed: " << *item << std::endl;
                } else {
                    std::cout << "Queue is empty" << std::endl;
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(15));
            }
        });
    }

    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers[i].join();
    }
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers[i].join();
    }
    
    return 0;
}