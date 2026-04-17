#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>

const int THINKING = 0;
const int HUNGRY = 1;
const int EATING = 2;

template <size_t NumP>
struct DiningTableTenenbaum {
    int N;
    std::mutex mut;
    std::vector<int> state; // state of each philosopher;
    // std::vector<std::counting_semaphore<1>> s; // one semaphore per philosopher
    std::vector<std::unique_ptr<std::counting_semaphore<1>>> s; // one semaphore per philosopher, using unique_ptr to avoid issues with non-copyable semaphores
    DiningTableTenenbaum() {
        N = NumP;
        state.resize(N, THINKING);
        // for (size_t i = 0; i < N; ++i) {
        //     s.emplace_back(0); // initialize semaphores to 0
        // }
        for (int i = 0; i < N; ++i) {
            s.emplace_back(std::make_unique<std::counting_semaphore<1>>(0));
        }
    }

    int left(int i) {
        return (i + N - 1) % N;
    }
    int right(int i) {
        return (i + 1) % N;
    }

    void philosopher(int i) {
        while (true) {
            // Think
            {
                std::scoped_lock<std::mutex> lock(mut);
                std::cout << "Philosopher " << i << " is thinking..." << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            takeChopsticks(i);
            
            // Eat
            {
                std::scoped_lock<std::mutex> lock(mut);
                std::cout << "Philosopher " << i << " is eating..." << std::endl;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            putChopsticks(i);
        }
    }

    void safeToEat(int i) {
        if (state[i] == HUNGRY && state[left(i)] != EATING && state[right(i)] != EATING) {
            state[i] = EATING;
            // s[i].release(); // signal philosopher i that they can eat
            s[i]->release(); // give permission to philosopher i signal philosopher i that they can eat
        }
    }

    void takeChopsticks(int i) {
        // Implementation of taking chopsticks (not shown)
        std::unique_lock<std::mutex> lock(mut);  // Use unique_lock for manual control
        state[i] = HUNGRY;
        
        // pick up chopsticks
        safeToEat(i);
        if (state[i] != EATING) {
            // If philosopher i cannot eat, they will wait on their semaphore
            lock.unlock(); // unlock before waiting to avoid deadlock
            s[i]->acquire(); // wait until philosopher i can eat
        }
    }

    void putChopsticks(int i) {
        std::scoped_lock<std::mutex> lock(mut);
        state[i] = THINKING;
        safeToEat(left(i));
        safeToEat(right(i));
    }
};


int main() {
    std::cout << "Starting Template-based Dining Philosophers Problem" << std::endl;
    std::cout << "Number of philosophers: 5" << std::endl;
    
    // Create philosopher threads
    std::vector<std::thread> philosophers;
    DiningTableTenenbaum<5> table;
    for (size_t i = 0; i < 5; i++) {
        philosophers.emplace_back(&DiningTableTenenbaum<5>::philosopher, &table, i);
    }
    
    // Wait for all threads to complete
    for (auto& t : philosophers) {
        t.join();
    }
    
    return 0;
}