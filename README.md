# C++ OrderBook Engine

A learning-focused C++ project that simulates the core behavior of a trading system’s order book and matching engine.

I built this project because I have an interest in trading systems and wanted to understand how orders are stored, matched, partially filled, and displayed inside a market-style engine. The goal is not to build a production exchange, but to learn the underlying mechanics behind limit orders, market orders, price-time priority, and bid/ask book structure.

---

## Overview

This project implements a basic order book using C++ standard library data structures.

The order book supports:

- Buy and sell orders
- Limit orders
- Market orders
- Price-time priority
- Partial fills
- Full fills
- Resting orders
- Best bid and best ask display
- Spread calculation
- Formatted terminal output
- Executed trade recording
- Shared trade history
- Individual trade printing
- Trade history display
- Integer-based price ticks instead of floating-point prices
- Interactive command-line order entry
- User-friendly price input such as `100.50`
- Basic validation for invalid price and quantity values

---

## Why This Project

Trading systems are a great way to practice systems thinking, data structures, and C++ fundamentals. Even a simple matching engine requires careful handling of ordering, priority, quantity updates, and edge cases.

This project helped me explore questions such as:

- How does an order book store buy and sell orders?
- How does a matching engine decide which orders trade first?
- What happens when an order is only partially filled?
- How are market orders different from limit orders?
- How can price-time priority be represented with C++ containers?
- Why are integer price ticks safer than floating-point prices in trading-style systems?
- How can a terminal interface guide users through order creation?

---

## Core Concepts

### Order

An order represents a request to buy or sell a quantity at a certain price.

Each order stores information such as:

- Order ID
- Order side: Buy or Sell
- Order type: Market or Limit
- Price ticks
- Quantity
- Sequence number
- Creation timestamp

### Price Ticks

Prices are stored as integer ticks using `std::uint64_t` instead of `double`.

This avoids floating-point precision issues when comparing, sorting, and matching prices.

For example, if the system treats one tick as one cent:

```text
10050 ticks = 100.50
10200 ticks = 102.00
```

Internally, the order book compares integer values like `10050` and `10200`, which is safer and more predictable than comparing decimal floating-point values.

The interactive CLI accepts user-friendly decimal-style input such as:

```text
100
100.5
100.50
99.75
```

and converts those values into price ticks before sending them to the order book.

### Order Book

The order book stores active buy and sell orders.

The buy side is sorted with the highest price first, because the best buyer is the one willing to pay the most.

The sell side is sorted with the lowest price first, because the best seller is the one willing to sell for the least.

### Matching Engine

The matching logic checks whether an incoming order can trade against existing orders on the opposite side of the book.

A buy limit order can match with sell orders at or below its limit price.

A sell limit order can match with buy orders at or above its limit price.

Market orders immediately consume available liquidity from the opposite side of the book. Any remaining unfilled quantity is cancelled.

### Trade

A trade represents one completed match between a buy order and a sell order.

Each trade stores information such as:

- Trade ID
- Buy order ID
- Sell order ID
- Execution price
- Executed quantity
- Aggressor side
- Execution timestamp

A single incoming order can create multiple trades if it matches against several resting orders across one or more price levels.

### Trade History

The trade history stores all executed trades in one shared location.

The `OrderBook` records trades into a shared `TradeHistory` object whenever a match occurs. This allows executed trades to be displayed separately from the current resting order book.

This separation keeps the design clear:

- `Order` represents a request to buy or sell.
- `OrderBook` stores active resting orders and performs matching.
- `Trade` represents one executed match.
- `TradeHistory` stores and displays all executed trades.

### Command-Line Interface

The project includes an interactive command-line interface in `main.cpp`.

The interface allows a user to:

- Place an order
- View the current order book
- View trade history
- Exit the program

When placing an order, the user is guided through:

1. Selecting the order side: Buy or Sell
2. Selecting the order type: Limit or Market
3. Entering quantity
4. Entering price if the order is a limit order

After an order is submitted, the screen is cleared and the updated order book and trade history are printed.

---

## Data Structures Used

The project uses `std::map` and `std::deque` to model the order book.

### Buy Orders

```cpp
std::map<std::uint64_t, std::deque<Order>, std::greater<std::uint64_t>> BuyOrders;
```

This stores buy orders grouped by price ticks, with the highest price first.

### Sell Orders

```cpp
std::map<std::uint64_t, std::deque<Order>> SellOrders;
```

This stores sell orders grouped by price ticks, with the lowest price first.

### Why `std::deque`

Each price level may contain multiple orders. A `std::deque` is used to preserve FIFO ordering at the same price level.

That means older orders at the same price are matched before newer orders.

### Trade History

Executed trades are stored in a vector:

```cpp
std::vector<Trade> trades;
```

A vector works well for trade history because trades are recorded as a growing list. Each new trade is appended to the end of the history.

The `TradeHistory` component manages trade IDs, creates new trade records, stores them, and prints the full trade history.

---

## Matching Behavior

### Limit Buy Example

If a buy limit order is placed at `10200` ticks, it can match with sell orders at:

```text
10125
10200
```

but not with sell orders above `10200`.

If one tick represents one cent, this means a buy limit at `102.00` can match with sells at `101.25` and `102.00`, but not above `102.00`.

### Limit Sell Example

If a sell limit order is placed at `10050` ticks, it can match with buy orders at:

```text
10050
10100
10200
```

but not with buy orders below `10050`.

If one tick represents one cent, this means a sell limit at `100.50` can match with buys at `100.50`, `101.00`, and `102.00`, but not below `100.50`.

### Market Buy Example

A market buy order consumes the best available sell orders until either:

- the market buy is fully filled, or
- there are no more sell orders available

Any remaining quantity is cancelled.

### Market Sell Example

A market sell order consumes the best available buy orders until either:

- the market sell is fully filled, or
- there are no more buy orders available

Any remaining quantity is cancelled.

---

## Order Handling Structure

The order book matching logic is split into helper methods for clarity:

- `HandleMarketBuy`
- `HandleMarketSell`
- `HandleLimitBuy`
- `HandleLimitSell`

The `AddOrder` method handles validation, creates the incoming order, and routes it to the correct handler based on side and type.

This keeps the high-level flow cleaner while keeping matching behavior inside the `OrderBook`.

---

## Price Input and Display

The CLI reads limit order prices as strings, then converts them into integer price ticks.

Examples:

```text
Input: 100      -> 10000 ticks
Input: 100.5    -> 10050 ticks
Input: 100.50   -> 10050 ticks
Input: 99.75    -> 9975 ticks
```

Prices can also be formatted back into display form with a dollar sign:

```text
10050 ticks -> $100.50
9975 ticks  -> $99.75
```

This keeps internal matching safe while still allowing the user to enter and view familiar price values.

---

## Example CLI Flow

```text
================ C++ ORDERBOOK ENGINE ================
1. Place Order
2. View Order Book
3. View Trade History
4. Exit
======================================================
Select an option: 1

Select Order Side:
1. Buy
2. Sell
Choice: 1

Select Order Type:
1. Limit
2. Market
Choice: 1

Enter Quantity: 10
Enter Limit Price: 100.50
```

After submission, the order book and trade history are displayed.

---

## Example Output

```text
TRADE: Buy Order 6 matched with Sell Order 3 | Price: 10125 | Quantity: 3 | Order Type: Limit
TRADE: Buy Order 6 matched with Sell Order 4 | Price: 10200 | Quantity: 12 | Order Type: Limit

############################################################
#                       ORDER BOOK                         #
############################################################

============================================================
  SELL SIDE / ASKS
============================================================
PRICE       QTY         ORDER ID    SEQ         LEVEL QTY
------------------------------------------------------------
No sell orders.

Spread unavailable. Both buy and sell sides need orders.

============================================================
  BUY SIDE / BIDS
============================================================
PRICE       QTY         ORDER ID    SEQ         LEVEL QTY
------------------------------------------------------------
10200       5           6           6           5
------------------------------------------------------------
10050       10          1           1           10
------------------------------------------------------------
9975        5           2           2           5
------------------------------------------------------------

====================== TRADE HISTORY ======================
Total Trades Executed: 3
-----------------------------------------------------------
=============== Trade: 1 ===============
Buy Order ID: 5
Sell Order ID: 3
Trade Price: 10125
Trade Quantity: 4
Aggressor Side: Buy
Executed At: 2026-05-31 20:59:54
========================================
```

---

## Project Structure

```text
cpp-orderbook-engine/
│
├── include/
│   ├── Enums.hpp
│   ├── Order.hpp
│   ├── OrderBook.hpp
│   ├── Trade.hpp
│   └── TradeHistory.hpp
│
├── src/
│   ├── Order.cpp
│   ├── OrderBook.cpp
│   ├── Trade.cpp
│   └── TradeHistory.cpp
│
├── tests/
│   └── TestLogic.cpp
│
├── .gitignore
├── main.cpp
└── README.md
```

---

## Building the Project

From the project root, compile with:

```bash
g++ main.cpp src/Order.cpp src/OrderBook.cpp src/Trade.cpp src/TradeHistory.cpp -I include -o main
```

Run with:

```bash
./main
```

On Windows PowerShell:

```powershell
.\main.exe
```

---

## Running the Test File

If using `tests/TestLogic.cpp`, compile with:

```bash
g++ tests/TestLogic.cpp src/Order.cpp src/OrderBook.cpp src/Trade.cpp src/TradeHistory.cpp -I include -o TestLogic
```

Run with:

```bash
./TestLogic
```

On Windows PowerShell:

```powershell
.\TestLogic.exe
```

---

## Current Features

- Limit order validation
- Market order handling
- Buy-side and sell-side matching
- Partial fill handling
- Full fill handling
- Price-level cleanup after orders are filled
- Remaining limit orders rest in the book
- Remaining market order quantity is cancelled
- Formatted terminal order book display
- Best bid, best ask, and spread display
- Integer-based price ticks using `std::uint64_t`
- Decimal-style user price input converted into price ticks
- Dollar-style price formatting for submitted order display
- Trade recording through a shared `TradeHistory` object
- Trade records with buy order ID, sell order ID, price ticks, quantity, aggressor side, and timestamp
- Individual trade printing
- Full trade history printing
- Separated order handling logic for market buys, market sells, limit buys, and limit sells
- Interactive CLI menu for placing orders and viewing book/history state

---

## Future Improvements

Possible next steps:

- Add order cancellation by order ID
- Add order lookup using a hash map
- Add unit tests
- Add support for modifying orders
- Add CSV export for order and trade history
- Add performance benchmarks
- Add support for configurable tick sizes
- Add formatted price display inside `OrderBook` and `TradeHistory`
- Add safer spread display for crossed-book debugging
- Add stronger validation for malformed price input
- Add support for multiple instruments/order books sharing one trade history

---

## What I Learned

This project helped me practice:

- C++ classes and headers
- Enum usage
- `std::map`
- `std::deque`
- `std::vector`
- References
- Iterators
- Integer-based price representation
- String-to-integer price parsing
- Price-time priority
- Matching engine logic
- Partial fills
- Trade history design
- Shared object references
- Constructor initializer lists
- Terminal formatting
- Multi-file C++ project structure
- Breaking large logic into smaller helper methods
- Building a simple command-line interface

---

## Disclaimer

This project is for educational purposes only. It is a simplified simulation of an order book and matching engine, not a production trading system or financial exchange.
