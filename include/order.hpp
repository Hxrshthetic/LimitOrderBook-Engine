#pragma once

#include <cstdint>

struct Order {
    int order_id;
    bool is_buy;
    bool is_market; // true = market order
    uint64_t price; // in cents (ignored if is_market)
    int quantity;

    Order(int id, bool is_buy, bool is_market, uint64_t price, int quantity);  // Constructor
};