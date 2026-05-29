#include <ctime>
#include <iomanip>
#include "order_book.hpp"
#include "timer.hpp"

void  OrderBook::matchOrders(Order& order) {
    if(order.is_buy) {
        auto it = asks.begin();
        while(order.quantity > 0 && it != asks.end()) {
            if(!order.is_market && order.price < it->first) {
                break; // cannot cross further asks
            }

            PriceLevel& level = it->second;
            auto order_it = level.orders.begin();
            while(order.quantity > 0 && order_it != level.orders.end()) {
                int fillQty = std::min(order.quantity, order_it->quantity);
                trades.emplace_back(level.price, fillQty);

                order.quantity -= fillQty;
                order_it->quantity -= fillQty;

                if(order_it->quantity == 0) {
                    uint64_t id = order_it->order_id;
                    order_it = level.orders.erase(order_it);
                    order_idx.erase(id);
                } else {
                    order_it++;
                }
            }
            if(level.orders.empty()) {
                it = asks.erase(it);
            } else {
                it++;
            }
        }
    } else {
        auto it = bids.begin();
        while(order.quantity > 0 && it != bids.end()) {
            if(!order.is_market && order.price > it->first) {
                break; // cannot cross further bids. Price too high
            }
            PriceLevel& level = it->second;
            auto order_it = level.orders.begin();
            while(order.quantity > 0 && order_it != level.orders.end()) {
                int fillQty = std::min(order.quantity, order_it->quantity);
                trades.emplace_back(level.price, fillQty);

                order.quantity -= fillQty;
                order_it->quantity -= fillQty;

                if(order_it->quantity == 0) {
                    uint64_t id = order_it->order_id;
                    order_it = level.orders.erase(order_it);
                    order_idx.erase(id);
                } else {
                    order_it++;
                }
            }
            if(level.orders.empty()) {
                it = bids.erase(it);
            } else {
                it++;
            }
        }
    }
}

void OrderBook::addOrder(const Order& order) {
    auto start = get_time();

    Order remaining = order;
    matchOrders(remaining);

    if(remaining.quantity > 0 && !remaining.is_market) {
        if(remaining.is_buy) {
            auto& level = bids[remaining.price];
            level.price = remaining.price;
            level.orders.emplace_back(remaining);
            auto list_it = std::prev(level.orders.end());
            auto map_it = bids.find(remaining.price);
            order_idx[remaining.order_id] = {true, map_it, list_it};
        } else {
            auto& level = asks[remaining.price];
            level.price = remaining.price;
            level.orders.emplace_back(remaining);
            auto list_it = std::prev(level.orders.end());
            auto map_it = asks.find(remaining.price);
            order_idx[remaining.order_id] = {false, map_it, list_it};
        }
    }

    auto end = get_time();
    double latency = end - start;

    total_orders++;
    if(order.is_buy) {
        buy_orders++;
        total_buy_latency += latency;
    } else {
        sell_orders++;
        total_sell_latency += latency;
    }

    if(order.is_market) {
        market_orders++;
        total_market_latency += latency;
    } else {
        limit_orders++;
        total_limit_latency += latency;
    }

    if(latency < 1.0) latency_bucket[0]++;
    else if(latency >= 1.0 && latency < 2.0) latency_bucket[1]++;
    else if(latency >= 2.0 && latency < 5.0) latency_bucket[2]++;
    else if(latency >= 5.0 && latency < 10.0) latency_bucket[3]++;
    else if(latency >= 10.0 && latency < 50.0) latency_bucket[4]++;
    else if(latency >= 50.0 && latency < 100.0) latency_bucket[5]++;
    else latency_bucket[6]++;

    total_latency += latency;
    if(latency < min_latency) min_latency = latency;
    if(latency > max_latency) max_latency = latency;
}

void OrderBook::cancelOrder(uint64_t id) {
    auto it = order_idx.find(id);
    if(it == order_idx.end()) {
        std::cout << "Order: " << id << " is not found.\n";
        return;
    }

    OrderLocation& location = order_idx[id];
    location.level_it->second.orders.erase(location.order_it);

    if(location.level_it->second.orders.empty()) {
        if(location.is_buy) {
            bids.erase(location.level_it);
        } else {
            asks.erase(location.level_it);
        }
    }

    order_idx.erase(it);
    std::cout << "Order " << id << " cancelled.\n";
}

void OrderBook::modifyOrder(uint64_t id, uint64_t new_price, int new_qty, bool is_market) {
    auto it = order_idx.find(id);
    if(it == order_idx.end()) {
        std::cout << "Order " << id << " not found.\n";
        return;
    }
    bool was_buy = it->second.is_buy;
    cancelOrder(id);

    Order new_order{id, was_buy, is_market, new_price, new_qty};
    addOrder(new_order);
}

void OrderBook::applyExecution(uint64_t id, int executed_shares) {
    if(executed_shares <= 0) return;
    auto it = order_idx.find(id);
    if(it == order_idx.end()) return;
    OrderLocation& location = it->second;
    Order& order = *location.order_it;
    int fillqty = std::min(order.quantity, executed_shares);
    trades.emplace_back(order.price, fillqty);
    order.quantity -= fillqty;
    if(order.quantity == 0) {
        location.level_it->second.orders.erase(location.order_it);
        if(location.level_it->second.orders.empty()) {
            if(order.is_buy) bids.erase(location.level_it);
            else asks.erase(location.level_it);
        }
        order_idx.erase(it);
    }
}

void OrderBook::printBook() const {
    std::cout << "\n--- Bids (buy orders) ---\n";
    for(const auto& [price, level]: bids) {
        std::cout << "Price (cents): " << price << ", Orders: ";
        for(const auto& order: level.orders) {
            std::cout << "(ID = " << order.order_id << ", Quantity = " << order.quantity << ") ";
        }
        std::cout << "\n";
    }

    std::cout << "\n--- Asks (sell orders) ---\n";
    for(const auto& [price, level]: asks) {
        std::cout << "Price (cents): " << price << " Orders: ";
        for(auto const& order: level.orders) {
            std::cout << "(ID = " << order.order_id << " Quantity = " << order.quantity << ")";
        }
        std::cout << "\n";
    }
}

void OrderBook::printTrades() const {
    std::cout << "\n--- Trades ---\n";
    for (const auto& trade : trades) {
        std::cout << "TRADE: price " << trade.first << " cents, quantity " << trade.second << "\n";
    }
}

void OrderBook::printMetrics() const {
    if (total_orders == 0) {
        std::cout << "No orders processed yet.\n";
        return;
    }
    double avg_us = total_latency / total_orders;
    auto now = std::chrono::high_resolution_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    std::cout << "Timestamp: " << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H-%M-%S") << "\n ";
    std::cout << "Orders processed: " << total_orders << "\n";
    std::cout << "Avg latency: " << avg_us << " µs\n";
    std::cout << "Min latency: " << min_latency << " µs\n";
    std::cout << "Max latency: " << max_latency << " µs\n\n";

    // Per-type breakdown
    if (market_orders > 0) {
        double market_avg = total_market_latency / market_orders;
        std::cout << "Market orders: " << market_orders << " (avg market latency " << market_avg << " µs)\n";
    }
    if (limit_orders > 0) {
        double limit_avg = total_limit_latency / limit_orders;
        std::cout << "Limit orders: " << limit_orders << " (avg limit order latency " << limit_avg << " µs)\n";
    }
    // Per-side
    if (buy_orders > 0) {
        double buy_avg = total_buy_latency / buy_orders;
        std::cout << "Buy orders: " << buy_orders << " (avg buy latency " << buy_avg << " µs)\n";
    }
    if (sell_orders > 0) {
        double sell_avg = total_sell_latency / sell_orders;
        std::cout << "Sell orders: " << sell_orders << " (avg sell latency " << sell_avg << " µs)\n";
    }

    // Histogram
    std::cout << "\n--- Latency Histogram (µs) ---\n";
    std::cout << "<1    : " << latency_bucket[0] << " orders\n";
    std::cout << "1-2   : " << latency_bucket[1] << " orders\n";
    std::cout << "2-5   : " << latency_bucket[2] << " orders\n";
    std::cout << "5-10  : " << latency_bucket[3] << " orders\n";
    std::cout << "10-50 : " << latency_bucket[4] << " orders\n";
    std::cout << "50-100: " << latency_bucket[5] << " orders\n";
    std::cout << ">100  : " << latency_bucket[6] << " orders\n";

    std::ifstream check("metrics.csv");
    bool empty = check.peek() == std::ifstream::traits_type::eof();
    check.close();

    std::ofstream file("metrics.csv", std::ios::app);
    if(!file.is_open()) return;

    if(empty) {
        file << "Timestamp, OrdersProcessed, AvgLatency_us, MinLatency_us, MaxLatency_us\n";
    }

    file << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H-%M-%S") << ", ";
    file << total_orders << ", ";
    file << avg_us << ", ";
    file << min_latency << ", ";
    file << max_latency << "\n";

    file.close();
}

void OrderBook::resetMetrics() {
    total_orders = 0;
    buy_orders = 0;
    sell_orders = 0;
    market_orders = 0;
    limit_orders = 0;

    total_latency = 0.0;
    total_buy_latency = 0.0;
    total_sell_latency = 0.0;
    total_limit_latency = 0.0;
    total_market_latency = 0.0;
    min_latency = 1e9;
    max_latency = 0.0;
}
