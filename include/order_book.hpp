#pragma once

#include <map>
#include <unordered_map>
#include <iostream>
#include <vector>
#include <cstdint>
#include <utility>
#include <chrono>
#include <fstream>
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
    std::unordered_map<uint64_t, OrderLocation> order_idx; // (id, location(asks? pricelevel? orderlist?))

    // Performance metrics
    int total_orders = 0;
    int market_orders = 0;
    int limit_orders = 0;
    int buy_orders = 0;
    int sell_orders = 0;

    double total_latency = 0.0;
    double total_market_latency  = 0.0;
    double total_limit_latency = 0.0;
    double total_buy_latency = 0.0;
    double total_sell_latency = 0.0;
    double min_latency = 1e9;
    double max_latency = 0.0;

    int latency_bucket[7] = {0}; // <1, 1-2, 2-5, 5-10, 10-20, 50-100, >100

    void matchOrders(Order& order); // core matching logic
    
public:
    void addOrder(const Order& order);
    void printBook() const;
    void  printTrades() const;
    void cancelOrder(uint64_t id);
    void modifyOrder(uint64_t id, uint64_t price, int qty, bool is_market);
    void applyExecution(uint64_t id, int executed_shares);
    void printMetrics() const;
    void resetMetrics();
};