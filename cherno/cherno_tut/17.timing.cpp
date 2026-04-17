#include <iostream>
#include <chrono>
#include <thread>

struct Timer {
    std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

    Timer() {
        start = std::chrono::high_resolution_clock::now();
    }

    ~Timer() {
        end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;
    }
};

int main() {
    // method 1: using std::chrono directly
    // auto start = std::chrono::high_resolution_clock::now(); // get the current time point
    // std::this_thread::sleep_for(std::chrono::seconds(1)); // sleep for 1 second
    // auto end = std::chrono::high_resolution_clock::now(); // get the current time point again
    // std::chrono::duration<double> elapsed = end - start; // calculate the elapsed time
    // std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl; // print the elapsed time in seconds
    // method 2: using a timer class, which will automatically start the timer when it is created, and automatically stop the timer and print the elapsed time when it is destroyed (when it goes out of scope)
    Timer timer; // create a timer object, which will automatically start the timer
    std::this_thread::sleep_for(std::chrono::seconds(1)); // sleep for 1 second
    return 0;
}