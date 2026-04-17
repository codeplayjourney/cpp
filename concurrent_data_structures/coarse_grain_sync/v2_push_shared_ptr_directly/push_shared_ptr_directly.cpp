#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>

// create shared ptr before we lock the mutex, so we don't hold the lock while creating the shared ptr, which can be time-consuming if T's constructor is expensive. This allows other threads to access the queue while we're creating the shared ptr, improving concurrency and reducing contention on the mutex.
template<typename T>
class QueueWithSharedPtr {
private:
    std::queue<std::shared_ptr<T>> data;
    std::mutex m;
    std::condition_variable cv;
public:
    void push(T value) {
        std::shared_ptr<T> ptr = std::make_shared<T>(std::move(value)); // Create shared_ptr from value
        std::lock_guard<std::mutex> lock(m);
        data.push(ptr);
        cv.notify_one(); // Notify one waiting thread
    }

    T pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this] { return !data.empty(); }); // Wait until data is available, check again if data is not empty in lambda function
        T value = *data.front();
        data.pop();
        return value;
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lock(m);
        cv.wait(lock, [this] { return !data.empty(); }); // Wait until data is available
        std::shared_ptr<T> res = data.front();
        data.pop();
        return res;
    }

    bool try_pop(T& value) {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            return false; // No data to pop
        }
        value = *data.front();
        data.pop();
        return true;
    }

    std::shared_ptr<T> try_pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            return std::shared_ptr<T>(); // Return empty shared_ptr if no data
        }
        std::shared_ptr<T> res = data.front();
        data.pop();
        return res;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

int main() {
    QueueWithSharedPtr<int> q;
    std::thread producer([&q]() { // Reference capture, starts thread and returns immediately
        for (int i = 0; i < 10; ++i) {
            q.push(i); // Push value directly, shared_ptr is created inside push method
            std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Simulate work
        }
    });

    // main thread continues immediately, starts consumer thread

    std::thread consumer([&q]() {
        for (int i = 0; i < 10; ++i) {
            std::shared_ptr<int> value = q.wait_and_pop();
            std::cout << "Popped: " << *value << std::endl;
        }
    });

    // Now main thread waits for both to finish
    producer.join();
    consumer.join();

    return 0;
}