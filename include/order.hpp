#pragma once

#include <cstdint>

struct Order {
    int order_id;
    bool is_buy;
    uint64_t price; // in cents (1$ = 100cents)
    int quantity;
};