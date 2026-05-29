#pragma once

#include <list>
#include <cstdint>
#include "order.hpp"

struct PriceLevel {
    uint64_t price; // in cents;
    std::list<Order> orders; // FIFO list

    PriceLevel(uint64_t price, std::list<Order> orders);
};