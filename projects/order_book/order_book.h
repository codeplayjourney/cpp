// order_book.h
#pragma once
#include <map>
#include <string>
#include <unordered_map>
#include "market_data.h"

struct PriceLevel {
    // struct: members are PUBLIC by default
    double price;
    int    quantity;
};

struct OrderBook {
    // bids — highest price first (buyer wants to pay as little as possible)
    std::map<double, int, std::greater<double>> bids;
    // asks — lowest price first (seller wants as much as possible)
    std::map<double, int> asks;

    void update(const MarketUpdate& u) {
        if (u.side == Side::Buy) {
            bids[u.price] += u.quantity;
        } else {
            asks[u.price] += u.quantity;
        }
    }

    void print(const char* symbol) const { // Arrays can't be passed by value in C/C++ so we use const char* for symbol, const because we won't modify the string through this pointer
        printf("\n=== %s ===\n", symbol);
        // print top 3 asks (lowest first)
        int count = 0;
        for (auto& [price, qty] : asks) {
            if (count++ >= 3) break;
            printf("  ASK  %.2f x %d\n", price, qty);
        }
        printf("  ─────────────\n");
        // print top 3 bids (highest first)
        count = 0;
        for (auto& [price, qty] : bids) {
            if (count++ >= 3) break;
            printf("  BID  %.2f x %d\n", price, qty);
        }
    }
};