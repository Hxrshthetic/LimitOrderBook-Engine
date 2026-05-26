#include "include/order_book.hpp"

int main() {
    OrderBook book;

    // Buy 100 shares at $100.50 (10050 cents)
    book.addOrder({1, true, 10050, 100});
    // Buy 200 shares at $99.75 (9975 cents)
    book.addOrder({2, true, 9975, 200});
    // Another buy at same price as above, should appear after ID=2 (FIFO)
    book.addOrder({4, true, 9975, 50});
    // Sell 50 shares at $101.25 (10125 cents)
    book.addOrder({3, false, 10125, 50});
    // Sell 30 shares at $102.00 (10200 cents)
    book.addOrder({5, false, 10200, 30});

    book.printBook();
    return 0;
}