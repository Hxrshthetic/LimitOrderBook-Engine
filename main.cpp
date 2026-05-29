#include <random>
#include "include/order_book.hpp"

int main() {
    OrderBook book;

    // Part 1: Basic order book demonstration (small test)
    std::cout << "=== Order Book Demo ===\n";

    // Add some resting orders
    book.addOrder({101, true, false, 10000, 50});   // buy 50 @ 100.00
    book.addOrder({102, true, false, 9900, 30});    // buy 30 @ 99.00
    book.addOrder({103, false, false, 10100, 40});  // sell 40 @ 101.00
    book.addOrder({104, false, false, 10200, 60});  // sell 60 @ 102.00
    book.printBook();

    // Add a crossing limit buy order (should match against ask at 101.00)
    std::cout << "\n--- Adding crossing buy order (ID 105) ---\n";
    book.addOrder({105, true, false, 10150, 30});   // buy 30 @ 101.50 (crosses 101.00 ask)
    book.printTrades();
    book.printBook();

    // Cancel an order
    std::cout << "\n--- Cancelling order ID 102 ---\n";
    book.cancelOrder(102);
    book.printBook();

    // Modify an order
    std::cout << "\n--- Modifying order ID 103: new price 101.50, qty 25 ---\n";
    book.modifyOrder(103, 10150, 25, false);
    book.printBook();

    // Add market order
    std::cout << "\n--- Adding market sell order (ID 106, qty 100) ---\n";
    book.addOrder({106, false, true, 0, 100});      // market sell 100
    book.printTrades();
    book.printBook();

    // Part 2: Performance metrics calculation
    std::cout << "\n=== Running Performance Benchmark ===\n";
    const int NUM_ORDERS = 10000;
    std::mt19937 rng(12345);

    std::uniform_int_distribution<int> side_dist(0, 1); // 0 = buy
    std::uniform_int_distribution<int> type_dist(0 , 4); // 25% market orders
    std::uniform_int_distribution<uint64_t> price_dist(9500, 10500); // $95.00 to $105.00
    std::uniform_int_distribution<int> qty_dist(1, 100);

    book.resetMetrics();

    for(uint64_t i = 0; i < NUM_ORDERS; i++) {
        uint64_t id = 2000 + i;
        bool is_buy = (side_dist(rng) == 0);
        bool is_market = (type_dist(rng) == 0);
        uint64_t price = is_market ? 0 : price_dist(rng);
        int qty = qty_dist(rng);

        book.addOrder({id, is_buy, is_market, price, qty});
    }
    // Print performance summary in metrics.csv
    book.printMetrics();

    std::cout << "Benchmark completed. Metrics written to metrics.csv\n";

    return 0;
}
