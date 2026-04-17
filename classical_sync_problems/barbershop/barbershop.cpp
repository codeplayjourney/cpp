#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>
using namespace std;

const int N = 5; // waiting room capacity

int customers = 0;
std::mutex mtx;
std::counting_semaphore<N> customer_sem(0);
std::counting_semaphore<1> barber_sem(0);
std::counting_semaphore<1> customerDone(0);
std::counting_semaphore<1> barberDone(0);
std::atomic<bool> running = true;

void getHairCut() {
    std::cout << "[Customer] Getting haircut...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void giveHairCut() {
    std::cout << "[Barber]   Giving haircut...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
}

void customer(int id) {
    {
        std::scoped_lock lock(mtx);
        if (customers == N) {
            std::cout << "[Customer " << id << "] Shop full, leaving.\n";
            return;
        }
        customers++;
        // scoped_lock will be auto released here due to raii
    }
    std::cout << "[Customer " << id << "] Entering shop, waiting...\n";
    barber_sem.acquire();       // wait(barber)
    customer_sem.release();     // signal(customer)

    getHairCut();

    customerDone.release();     // signal(customerDone)
    barberDone.acquire();       // wait(barberDone)

    {
        std::scoped_lock lock(mtx);
        customers--;
    }

    std::cout << "[Customer " << id << "] Done, leaving.\n";
}

void barber() {
    while (running) {
        barber_sem.release();       // signal(barber)
        customer_sem.acquire();     // wait(customer)

        giveHairCut();

        customerDone.acquire();     // wait(customerDone)
        barberDone.release();       // signal(barberDone)
    }
}

int main() {
    std::thread barber_thread(barber);

    std::vector<std::thread> threads;
    for (int i = 0; i < 8; i++) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        threads.emplace_back(customer, i);
    }

    // join customer threads, to ensure main doesnt return before they finish
    for (auto& t : threads) t.join();
    running = false; // stop barber loop
    
    barber_thread.detach(); // detach since barber will exit on its own when running is false
    /* alternative to detaching the barber thread, but have redundant releases and print another haircut after the last customer leaves
    // customer_sem.release(); // unblock barber from customer_sem.acquire()
    // customerDone.release();     // wait(customerDone)
    // barber_thread.join(); // clean join
    */
    return 0;
}