#include "price_level.hpp"

PriceLevel::PriceLevel(uint64_t price, std::list<Order> orders)
    : price(price), orders(orders) {}