# Limit Order Book Engine

> A high-performance limit order book and matching engine for low-latency trading simulations.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [Performance Metrics](#performance-metrics)
- [Milestones](#milestones)
- [License](#license)

---

## Overview

**Limit Order Book Engine** is a C++17 implementation of a price-time priority order book and matching engine designed for low-latency trading simulations. It supports limit and market orders, partial fills, and full trade logging — all using integer cent-based pricing to eliminate floating-point precision issues.

---

## Features

- **Price-Time Priority (FIFO)** — Orders at the same price level are matched in the order they were received.
- **Limit Orders & Market Orders** — Submit resting limit orders or aggressive market orders that match immediately.
- **Matching Engine** — Automatically crosses buy and sell orders whenever prices overlap.
- **Partial Fills** — Unfilled quantity from a partially matched order remains resting in the book.
- **Order Cancellation** — Cancel any resting order by its unique ID with O(1) lookup.
- **Order Modification** — Change price, quantity, or order type (limit/market) of an existing order (atomic cancel + replace).
- **Trade Recording & Logging** — Every fill is recorded with price, quantity, and aggressor side.
- **Cents-Based Integer Pricing** — Prices are represented as integers (e.g., `10050` = $100.50) to avoid all floating-point issues.
- **High-Resolution Latency Metrics** — Per-order latency tracking using `QueryPerformanceCounter` on Windows (microsecond accuracy) with min/max/average breakdowns across order types and sides.
- **Latency Histogram** — Bucketed distribution across `<1`, `1–2`, `2–5`, `5–10`, `10–50`, `50–100`, and `>100 µs` ranges.
- **CSV Export** — Appends a summary line (timestamp, total orders, avg/min/max latency) to `metrics.csv` after each run.

---

## Project Structure

```
LimitOrderBookEngine/
├── include/          # Header files
├── src/              # Implementation files
├── tests/            # Unit tests (future)
├── main.cpp          # Example usage
├── metrics.csv       # Latency summary log (created if does not exists, auto-appended on each run)
└── CMakeLists.txt    # Build configuration
```

---

## Build Instructions

### Prerequisites

- [CMake](https://cmake.org/) 3.20 or higher
- A C++17-compatible compiler: **GCC**, **Clang**, or **MSVC**

### Command Line

```bash
mkdir build && cd build
cmake ..
make
./orderbook
```

### VS Code (CMake Tools Extension)

1. Open the project folder in VS Code.
2. Open the Command Palette and select **CMake: Select a Kit** — choose your **GCC** kit.
3. Press **F7** to configure and build.
4. Run the resulting binary from the terminal or the CMake Tools run button.

---

## Usage

```cpp
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

    for(int i = 0; i < NUM_ORDERS; i++) {
        int id = 2000 + i;
        bool is_buy = (side_dist(rng) == 0);
        bool is_market = (type_dist(rng) == 0);
        uint64_t price = is_market ? 0 : price_dist(rng);
        int qty = qty_dist(rng);

        book.addOrder({id, is_buy, is_market, price, qty});
    }
    // Print performance summary in metrics.csv and console
    book.printMetrics();

    std::cout << "Benchmark completed. Metrics written to metrics.csv\n";

    return 0;
}
```

**Order struct fields:** `{ order_id, is_buy, is_market, price, quantity }`

| Field       | Type   | Description                                  |
|-------------|--------|----------------------------------------------|
| `order_id`  | `int`  | Unique identifier for the order              |
| `is_buy`    | `bool` | `true` = buy side, `false` = sell side       |
| `is_market` | `bool` | `true` = market order, `false` = limit order |
| `price`     | `int`  | Price in cents (e.g., `10050` = $100.50)     |
| `quantity`  | `int`  | Number of shares/contracts                   |

---

## Performance Metrics

Every `addOrder` call is timed using `QueryPerformanceCounter` (Windows) for microsecond-level accuracy. Calling `printMetrics()` outputs a full breakdown to the console and appends a summary row to `metrics.csv`.

### Console Output

```
=== Running Performance Benchmark ===
Timestamp: 2026-05-28 23-44-18
 Orders processed: 10000
Avg latency: 1.40729 µs
Min latency: 0.0998535 µs
Max latency: 124.8 µs

Market orders: 2054 (avg market latency 1.16709 µs)
Limit orders: 7946 (avg limit order latency 1.46938 µs)
Buy orders: 5056 (avg buy latency 1.43204 µs)
Sell orders: 4944 (avg sell latency 1.38198 µs)

--- Latency Histogram (µs) ---
<1    : 4091 orders
1-2   : 3614 orders
2-5   : 2236 orders
5-10  : 32 orders
10-50 : 28 orders
50-100: 4 orders
>100  : 2 orders
```

#### Note: 
The character `µ` for micro could be printed as `┬╡` in console. It can be fixed by replacing it with letter `u` in `printMetrics()` (src/order_book.cpp) or ensuring UTF-8 output. 

### CSV Export (`metrics.csv`)

Each run appends one row:

```
Timestamp, OrdersProcessed, AvgLatency_us, MinLatency_us, MaxLatency_us
2026-05-29 00-03-01, 10000, 0.674541, 0, 119.1
```

### API

| Method | Description |
|---|---|
| `printMetrics()` | Prints breakdown + histogram to stdout; appends summary to `metrics.csv` |
| `resetMetrics()` | Clears all accumulated latency data for a fresh measurement window |

---

## Milestones

- [x] Basic order book with `addOrder` and `printBook`
- [x] Matching engine with market order support
- [x] Order cancellation & modification
- [x] Performance metrics (latency / throughput benchmarks)

---

## License

This project is licensed under the [MIT License](LICENSE).