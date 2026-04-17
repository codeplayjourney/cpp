#include <iostream>
#include <stack>
#include <mutex>

template<typename T>
class threadsafe_stack {
private:
    std::stack<T> data;
    mutable std::mutex m;
public:
    void push(T value) {
        std::lock_guard<std::mutex> lock(m);
        data.push(value);
    }

    std::unique_ptr<T> pop() {
        std::lock_guard<std::mutex> lock(m);
        if (data.empty()) {
            throw std::runtime_error("Stack is empty");
        }
        std::unique_ptr<T> res(std::make_unique<T>(std::move(data.top())));
        data.pop();
        return res;
    }

    bool empty() {
        std::lock_guard<std::mutex> lock(m);
        return data.empty();
    }
};

#include <thread>
#include <vector>
#include <atomic>

int main() {
    threadsafe_stack<int> s;
    std::vector<std::thread> threads;
    std::atomic<bool> done{false};

    // Push elements in a separate thread
    for (int i = 0; i < 1000; i++) {
        threads.emplace_back([&s, i]() {
            s.push(i);
        });
    }
    for (auto& t : threads) {
        t.join();  // ← This ensures thread finished
    }
    done = true;

    // Pop elements in the main thread
    while (!done || !s.empty()) {
        try {
            auto value = s.pop();
            if (value) {
                std::cout << "Popped: " << *value << std::endl;
            }
        } catch (const std::runtime_error& e) {
            // Stack is empty, wait for more elements to be pushed
        }
    }


    return 0;
}