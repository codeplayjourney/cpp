// note: this implementation might not pop all elements since we do not wait and pop
#include <iostream>
#include <mutex>
#include <thread>

template<typename T>
class MultithreadedQueueWithDummyNode {
private:
    struct Node {
        T data;
        std::unique_ptr<Node> next;
        Node(T value) : data(value), next(nullptr) {} // member initializer list syntax
    };
    std::unique_ptr<Node> front;
    Node* back;
    std::mutex back_mutex; // mutex to protect access to the queue
    std::mutex front_mutex; // mutex to protect access to the queue

public:
    MultithreadedQueueWithDummyNode() : front(std::make_unique<Node>(T())), back(front.get()) {}
    // pop: we pop the first node and shift front to point to first node->next
    // push(new data): we change data to be the new data and make next point to new dummy node
    void push(T value) {
        std::lock_guard<std::mutex> lock(back_mutex); // lock the mutex to protect access to the queue
        back->data = std::move(value);
        std::unique_ptr<Node> newDummyNode = std::make_unique<Node>(T());
        back->next = std::move(newDummyNode);
        // point back to the new dummy node
        back = back->next.get();
    }

    std::shared_ptr<T> try_pop() {
        std::unique_lock<std::mutex> frontLock(front_mutex); // lock the mutex to protect access to the queue
        std::lock_guard<std::mutex> backLock(back_mutex); // lock the back mutex to check if queue is empty
        if (front.get() == back) {
            // check if queue only contains dummy node
            return std::shared_ptr<T>(); // return empty shared_ptr if queue is empty
        }
        backLock.unlock(); // unlock the back mutex
        // dont auto suggest let me try first
        std::shared_ptr<T> res = std::make_shared<T>(std::move(front->data));
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