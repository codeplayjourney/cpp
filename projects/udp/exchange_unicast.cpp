#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <netdb.h>
#include "market_data.h"
#include <atomic>
#include <csignal>

#define PORT     "9000"
// #define HOST     "192.168.1.9"  // change to your local IP address
#define HOST     "localhost"

// get current time in nanoseconds
uint64_t now_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

// simulate a price that moves randomly around a base price
double next_price(double last_price) {
    double move = ((rand() % 21) - 10) * 0.01;  // -0.10 to +0.10
    return last_price + move;
}

std::atomic<bool> running{true}; // Volatile-like visibility — forces the compiler to always read from memory, never cache in a register. So the main loop always sees the signal handler's write.
void signal_handler(int signal) {
    running = false;
}

int main() {
    std::signal(SIGINT, signal_handler); // which signal, what fn to call when signal arrives
    srand(42);

    // setup socket
    struct addrinfo hints, *res;
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    if (getaddrinfo(HOST, PORT, &hints, &res) != 0) {
        perror("getaddrinfo");
        return 1;
    }

    int sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    // simulate 3 symbols
    const char* symbols[] = { "AAPL", "MSFT", "GOOG" };
    double prices[]       = { 182.50, 415.20, 140.30 };
    int    num_symbols    = 3;

    printf("exchange: sending market data to %s:%s\n", HOST, PORT);

    int seq = 0;
    while (running) {
        // pick a random symbol
        int i = rand() % num_symbols;

        // build the update
        MarketUpdate update;
        update.timestamp = now_ns();
        strncpy(update.symbol, symbols[i], sizeof update.symbol);
        prices[i]        = next_price(prices[i]);
        update.price     = prices[i];
        update.quantity  = (rand() % 10 + 1) * 100;  // 100 to 1000
        update.side      = (rand() % 2) ? Side::Buy : Side::Sell;

        // send it
        int bytes = sendto(sock, &update, sizeof update, 0,
                           res->ai_addr, res->ai_addrlen);
        if (bytes == -1) {
            perror("sendto");
            break;
        }

        printf("exchange: sent %s %.2f x %d %c\n",
               update.symbol, update.price,
               update.quantity, update.side);

        seq++;
        usleep(10000);  // 10ms between updates = 100 updates/sec
    }

    freeaddrinfo(res);
    close(sock);
    return 0;
}