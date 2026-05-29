#include "order.hpp"

Order::Order(int id, bool is_buy, bool is_market, uint64_t price, int qty)
    : order_id(id), is_buy(is_buy), is_market(is_market), price(price), quantity(qty) {}