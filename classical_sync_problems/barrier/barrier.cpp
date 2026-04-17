#include <iostream>
#include <thread>
#include <barrier>
#include <chrono>
#include <semaphore>
#include <mutex>
using namespace std;
std::mutex mut;
std::atomic<int> counter = 0;
std::atomic<int> expected;
std::counting_semaphore<1> turnstile(0);
std::counting_semaphore<1> turnstile2(1);

struct Barrier {
    Barrier(int num) {
        expected = num;
    }


    void arrive_and_wait(int tid) { // need turnstile2 to prevent threads from passing through the barrier before all threads have arrived
        // Simulate work before the barrier
        {
            std::scoped_lock lock{mut};
            counter++;
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            // std::cout << "Thread " << std::this_thread::get_id() << " arrived at the barrier." << std::endl;
            std::cout << "Thread " << tid << " arrived at the barrier." << std::endl;

            if (counter == expected) {
                turnstile2.acquire();
                turnstile.release();
            }
        }
        turnstile.acquire(); // block until all threads have arrived at the barrier
        turnstile.release(); // Allow the next thread to pass through the barrier
        {
            std::scoped_lock lock{mut};
            counter--;
            if (counter == 0) {
                turnstile.acquire(); // close turnstile to reset barrier
                // open second turnstile to allow threads to pass through
                turnstile2.release();
            }
        }
        turnstile2.acquire();
        turnstile2.release();
    }
    void dummyWait(int tid) {
        // Simulate work of thread
        {
            std::scoped_lock lock{mut};
            std::cout << "Thread " << tid << " executing work." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        arrive_and_wait(tid);
        {
            std::scoped_lock lock{mut};
            std::cout << "Thread " << tid << " executing work 2." << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        arrive_and_wait(tid);
    }
};


int main() {
    int numThreads = 5;
    Barrier barrier(numThreads);
    std::vector<std::thread> threads;
    for (size_t i = 0; i < 5; i++) {
        // 1. Function pointer 2. Object 3. Parameter
        threads.emplace_back(&Barrier::dummyWait, &barrier, i);
    }
    
    // Wait for all threads to complete
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "All threads passed the barrier." << std::endl;

    return 0;
}