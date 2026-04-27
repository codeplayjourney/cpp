#pragma once // prevents the same header file from being included twice in the same compilation
#include <cstdint>
/*
The struct is what gets sent as raw bytes over UDP — like Beej's buf but structured
*/

enum class Side : char { // use char as underlying type to save space, since we only need 'B' or 'S'
    Buy  = 'B',
    Sell = 'S'
};

struct MarketUpdate {
    uint64_t timestamp;   // nanoseconds since epoch (8 bytes)
    char     symbol[8];   // e.g. "AAPL" (8 bytes, fixed size for simplicity)
    double   price; // 8 bytes
    int32_t      quantity; // 4 bytes
    Side     side;        // 'B' buy or 'S' sell // 1 byte
};