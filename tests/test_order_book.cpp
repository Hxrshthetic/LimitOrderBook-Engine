#include "order_book.hpp"

int main() {
    OrderBook book;
    book.addOrder({1, true, 10000, 100});
    book.printBook();
    return 0;
}