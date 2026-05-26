#pragma once

#include <map>
#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>
#include "price_level.hpp"

class OrderBook {
private:
    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids; // Descending price (highest first)
    std::map<uint64_t, PriceLevel> asks; // Ascending price (lowest first)

public:
    void addOrder(const Order& order);
    void printBook() const;
};