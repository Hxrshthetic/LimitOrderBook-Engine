# Limit Order Book Engine

> A high-performance limit order book and matching engine for low-latency trading simulations.

---

## Table of Contents

- [Overview](#overview)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
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
- **Trade Recording & Logging** — Every fill is recorded with price, quantity, and aggressor side.
- **Cents-Based Integer Pricing** — Prices are represented as integers (e.g., `10050` = $100.50) to avoid all floating-point issues.

---

## Project Structure

```
LimitOrderBookEngine/
├── include/          # Header files
├── src/              # Implementation files
├── tests/            # Unit tests (future)
├── main.cpp          # Example usage
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
#include "orderbook.h"

int main() {
    OrderBook book;

    // Buy 100 shares @ $100.50 (limit)
    book.addOrder({1, true, false, 10050, 100});

    // Sell 50 shares @ $101.25 (limit)
    book.addOrder({2, false, false, 10125, 50});

    // Buy 80 shares @ $102.00 — crosses the spread, triggers a match
    book.addOrder({3, true, false, 10200, 80});

    book.printTrade();
    book.printBook();

    return 0;
}
```

**Order struct fields:** `{ id, is_buy, is_market, price, quantity }`

| Field       | Type   | Description                                  |
|-------------|--------|----------------------------------------------|
| `id`        | `int`  | Unique identifier for the order              |
| `is_buy`    | `bool` | `true` = buy side, `false` = sell side       |
| `is_market` | `bool` | `true` = market order, `false` = limit order |
| `price`     | `int`  | Price in cents (e.g., `10050` = $100.50)     |
| `quantity`  | `int`  | Number of shares/contracts                   |

---

## Milestones

- [x] Basic order book with `addOrder` and `printBook`
- [x] Matching engine with market order support
- [ ] Order cancellation & modification
- [ ] Performance metrics (latency / throughput benchmarks)

---

## License

This project is licensed under the [MIT License](LICENSE).