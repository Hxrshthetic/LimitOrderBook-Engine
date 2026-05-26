#include "include/order_book.hpp"

int main() {
    OrderBook book;

    book.addOrder({1, true, false, 9900, 100});   // buy 100 @ 99.00
    book.addOrder({2, false, false, 10100, 50});  // sell 50 @ 101.00

    book.printBook();

    // Crossing buy limit order (buys at 102.00, should match against ask at 101.00)
    book.addOrder({3, true, false, 10200, 80});   // buy 80 @ 102.00

    book.printBook();
    book.printTrades();

    // Market sell order (sells 60 shares)
    book.addOrder({4, false, true, 0, 60});       // market sell

    book.printBook();
    book.printTrades();

    return 0;
}
