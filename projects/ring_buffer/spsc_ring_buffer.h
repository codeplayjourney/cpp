#pragma once
#include <atomic>
#include <array>
#include <optional>
#include <cstddef>

/*
The memory ordering
cpp// producer
w = write_pos_.load(std::memory_order_relaxed);  // reading OWN variable — relaxed ok
read_pos_.load(std::memory_order_acquire);        // reading SHARED variable — acquire
write_pos_.store(next_w, std::memory_order_release); // publishing to consumer — release

// consumer
r = read_pos_.load(std::memory_order_relaxed);   // reading OWN variable — relaxed ok
write_pos_.load(std::memory_order_acquire);       // reading SHARED variable — acquire
read_pos_.store(..., std::memory_order_release);  // publishing to producer — release
Relaxed — reading your own variable, no synchronisation needed.
Acquire — "I'm about to read data written by the other thread, make sure I see it".
Release — "I've written data, make sure the other thread sees it".
*/
template<typename T, std::size_t Capacity>
class SPSCRingBuffer {
    // capacity must be power of 2 so we can use bitwise AND for wraparound
    // instead of slow modulo operation
    /*
    e.g.
    Capacity - 1 = 7 = 0b00000111
    (8 + 1) & 7  = 9 & 7 = 0b00001001 & 0b00000111 = 0b00000001 = 1  ← wraps to 1
    */
   // it is power of 2 if and only if there is 1 bit except for LSB is set to 1
    static_assert((Capacity & (Capacity - 1)) == 0,
                  "Capacity must be power of 2");

    std::array<T, Capacity> buffer_; // trailing underscore is a common convention for private member variables in C++

    // alignas(64) puts each atomic on its own cache line (most hardware architectures like x86's cache line size is 64 bytes)
    // prevents false sharing — both threads invalidating each other's cache
    // alignas(N) forces a variable to start at a memory address divisible by N:
    alignas(64) std::atomic<std::size_t> write_pos_{0};
    alignas(64) std::atomic<std::size_t> read_pos_{0};

public:
    // called by producer thread ONLY
    bool push(const T& item) {
        std::size_t w = write_pos_.load(std::memory_order_relaxed);
        // next write position after this one
        std::size_t next_w = (w + 1) & (Capacity - 1);

        // buffer full if next write position == read position
        if (next_w == read_pos_.load(std::memory_order_acquire)) {
            return false;  // full, drop the item
        }

        buffer_[w] = item;

        // release — makes the write visible to consumer
        write_pos_.store(next_w, std::memory_order_release);
        return true;
    }

    // called by consumer thread ONLY
    std::optional<T> pop() {
        std::size_t r = read_pos_.load(std::memory_order_relaxed);

        // buffer empty if read position == write position
        if (r == write_pos_.load(std::memory_order_acquire)) {
            return std::nullopt;  // empty
        }

        T item = buffer_[r];

        // release — makes the read visible to producer
        read_pos_.store((r + 1) & (Capacity - 1), std::memory_order_release);
        return item;
    }

    bool empty() const {
        return read_pos_.load(std::memory_order_acquire) ==
               write_pos_.load(std::memory_order_acquire);
    }
};