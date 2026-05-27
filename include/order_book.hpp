#pragma once

#include <map>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>
#include "price_level.hpp"

struct OrderLocation {
    bool is_buy; // true for bid;
    std::map<uint64_t, PriceLevel>::iterator level_it; // iterator for specific PriceLevel.
    std::list<Order>::iterator order_it;
};

class OrderBook {
private:
    std::map<uint64_t, PriceLevel, std::greater<uint64_t>> bids; // Descending price (highest first)
    std::map<uint64_t, PriceLevel> asks; // Ascending price (lowest first)
    std::vector<std::pair<uint64_t, int>> trades; // (price, qty)
    std::unordered_map<int, OrderLocation> order_idx; // (id, location(asks? pricelevel? orderlist?))

    void matchOrders(Order& order); // core matching logic
    
public:
    void addOrder(const Order& order);
    void printBook() const;
    void  printTrades() const;
    void cancelOrder(int id);
    void modifyOrder(int id, uint64_t price, int qty, bool is_market);
};