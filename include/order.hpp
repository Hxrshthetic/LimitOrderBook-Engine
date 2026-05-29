#pragma once

#include <cstdint>

struct Order {
    uint64_t order_id;
    bool is_buy;
    bool is_market; // true = market order
    uint64_t price; // in cents (ignored if is_market)
    int quantity;

    Order(uint64_t id, bool is_buy, bool is_market, uint64_t price, int quantity);  // Constructor
};