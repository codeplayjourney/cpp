#include <thread>
#include <iostream>
#include <cassert>
#include "spsc_ring_buffer.h"

int main() {
    SPSCRingBuffer<int, 64> queue;

    // producer thread
    std::thread producer([&]() { // capture everything outside by reference
        for (int i = 0; i < 100; i++) {
            while (!queue.push(i)) {}  // spin if full
        }
    });

    // consumer thread
    std::thread consumer([&]() {
        int expected = 0;
        while (expected < 100) {
            auto val = queue.pop(); // returns std::optional<int>, which can either contain an int value or be empty (std::nullopt)
            if (val) {
                assert(*val == expected);
                expected++;
            }
        }
        std::cout << "all 100 items received correctly\n";
    });

    producer.join();
    consumer.join();
    return 0;
}