#include "order_book.hpp"

void OrderBook::addOrder(const Order& order) {

    if(order.is_buy) {
        auto& level = bids[order.price];
        level.price = order.price;
        level.orders.emplace_back(order);
    } else {
        auto& level = asks[order.price];
        level.price = order.price;
        level.orders.emplace_back(order);
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