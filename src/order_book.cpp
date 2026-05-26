#include "order_book.hpp"

void OrderBook::addOrder(const Order& order) {
    Order remaining = order;
    matchOrders(remaining);

    if(remaining.quantity > 0 && !remaining.is_market) {
        if(remaining.is_buy) {
            auto& level = bids[remaining.price];
            level.price = remaining.price;
            level.orders.emplace_back(remaining);
        } else {
            auto& level = asks[remaining.price];
            level.price = remaining.price;
            level.orders.emplace_back(remaining);
        }
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
                    order_it = level.orders.erase(order_it);
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
                    order_it = level.orders.erase(order_it);
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
