#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>
#include <random>
#include <csignal>
#include <atomic>
using namespace std;

const int NumP = 5; // number of philosophers

// pseudocode:
/*
template <size_t NumP>
struct DiningTable2 {
    using Chopstick = std::mutex;
    Chopstick chopsticks[NumP];
    std::counting_semaphore<NumP - 1> footman_semaphore{NumP - 1}; // footman allows up to NumP-1 philosophers to eat

    // pid = philosopher id
    Chopstick& left_chopstick(size_t pid) {
        return chopsticks[pid];
    }
    Chopstick& right_chopstick(size_t pid) {
        return chopsticks[(pid + 1) % NumP];
    }
    
    void eat(size_t pid) {
        footman_semaphore.acquire(); // request permission from footman
        left_chopstick(pid).lock(); // pick up left chopstick
        right_chopstick(pid).lock(); // pick up right chopstick

        // eat (simulate with sleep)
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        right_chopstick(pid).unlock(); // put down right chopstick
        left_chopstick(pid).unlock(); // put down left chopstick
        footman_semaphore.release(); // notify footman that philosopher is done eating
    }
}
*/

template <size_t NumP>
struct DiningTable2 {
    using Chopstick = std::mutex;
    Chopstick chopsticks[NumP];
    std::counting_semaphore<NumP - 1> footman_semaphore{NumP - 1}; // footman allows up to NumP-1 philosophers to eat

    // pid = philosopher id
    Chopstick& left_chopstick(size_t pid) {
        // returns chopstick&
        // Without reference (if it returned by value):
        // This would be WRONG - creates copies of mutexes!
        // std::scoped_lock lock(left_chopstick(pid), right_chopstick(pid));
        return chopsticks[pid];
    }
    
    Chopstick& right_chopstick(size_t pid) {
        return chopsticks[(pid + 1) % NumP];
    }
    
    void eat(size_t pid) {
        footman_semaphore.acquire(); // request permission from footman
        std::scoped_lock left_lock(left_chopstick(pid));
        std::scoped_lock right_lock(right_chopstick(pid));
        
        // eat (simulate with sleep)
        cout << "Philosopher " << pid << " is eating..." << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        cout << "Philosopher " << pid << " finished eating." << endl;

        footman_semaphore.release(); // notify footman that philosopher is done eating
    }
};

// Global dining table for 5 philosophers
DiningTable2<5> table;

// Random number generator for thinking time
thread_local std::random_device rd;
thread_local std::mt19937 gen(rd());
thread_local std::uniform_int_distribution<> think_time(50, 200); // 50-200ms

void philosopher(size_t pid) {
    // while (true) {
        // Think
        cout << "Philosopher " << pid << " is thinking..." << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(think_time(gen)));
        
        // Eat using the dining table
        table.eat(pid);
    // }
}

int main() {
    cout << "Starting Template-based Dining Philosophers Problem" << endl;
    cout << "Number of philosophers: 5" << endl;
    cout << "Footman allows at most 4 philosophers to eat simultaneously" << endl;
    
    // Create philosopher threads
    vector<thread> philosophers;
    for (size_t i = 0; i < 5; i++) {
        philosophers.emplace_back(philosopher, i);
    }
    
    // Wait for all threads to complete
    for (auto& t : philosophers) {
        t.join();
    }
    
    return 0;
}