#include <iostream>
#include <iterator>
#include <string>

#include "OrderBook.hpp"
#include "Utils.hpp"

OrderBook::OrderBook(const std::string &symbol) : symbol(symbol)
{
    nextOrderId = 1;
    nextOrderSequenceNumber = 1;
}

void OrderBook::HandleMarketBuy(Order &NewOrder)
{
    int OriginalQuantity = NewOrder.quantity;

    while (NewOrder.quantity > 0 && !SellOrders.empty())
    {
        auto BestSell = SellOrders.begin();
        std::uint64_t BestSellPrice = BestSell->first;

        std::list<Order> &OrdersAtBestSellPrice = BestSell->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
        {
            Order &BestSellOrder = OrdersAtBestSellPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestSellOrder.quantity -= TradeQuantity;

            tradeHistory.AddTrade(symbol, NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy);
            std::cout << "TRADE: Buy Order " << NewOrder.id << " matched with Sell Order " << BestSellOrder.id << " | Price: " << FormatPrice(BestSellPrice) << " | Quantity: " << TradeQuantity << " | Order Type: Market" << std::endl;

            if (BestSellOrder.quantity == 0)
            {
                OrderLocations.erase(BestSellOrder.id);
                OrdersAtBestSellPrice.pop_front();
            }
        }

        if (OrdersAtBestSellPrice.empty())
        {
            SellOrders.erase(BestSell);
        }
    }

    if (NewOrder.quantity > 0)
    {
        if (NewOrder.quantity == OriginalQuantity)
        {
            std::cout << "MARKET BUY ORDER " << NewOrder.id << " could not be filled. Full quantity cancelled: " << NewOrder.quantity << std::endl;
        }
        else
        {
            std::cout << "MARKET BUY ORDER " << NewOrder.id << " partially filled. Unfilled quantity cancelled: " << NewOrder.quantity << std::endl;
        }
    }
}

void OrderBook::HandleMarketSell(Order &NewOrder)
{
    int OriginalQuantity = NewOrder.quantity;

    while (NewOrder.quantity > 0 && !BuyOrders.empty())
    {
        auto BestBuy = BuyOrders.begin();
        std::uint64_t BestBuyPrice = BestBuy->first;

        std::list<Order> &OrdersAtBestBuyPrice = BestBuy->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
        {
            Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestBuyOrder.quantity -= TradeQuantity;

            tradeHistory.AddTrade(symbol, BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell);
            std::cout << "TRADE: Sell Order " << NewOrder.id << " matched with Buy Order " << BestBuyOrder.id << " | Price: " << FormatPrice(BestBuyPrice) << " | Quantity: " << TradeQuantity << " | Order Type: Market" << std::endl;

            if (BestBuyOrder.quantity == 0)
            {
                OrderLocations.erase(BestBuyOrder.id);
                OrdersAtBestBuyPrice.pop_front();
            }
        }

        if (OrdersAtBestBuyPrice.empty())
        {
            BuyOrders.erase(BestBuy);
        }
    }

    if (NewOrder.quantity > 0)
    {
        if (NewOrder.quantity == OriginalQuantity)
        {
            std::cout << "MARKET SELL ORDER " << NewOrder.id << " could not be filled. Full quantity cancelled: " << NewOrder.quantity << std::endl;
        }
        else
        {
            std::cout << "MARKET SELL ORDER " << NewOrder.id << " partially filled. Unfilled quantity cancelled: " << NewOrder.quantity << std::endl;
        }
    }
}

void OrderBook::HandleLimitBuy(Order &NewOrder)
{
    while (NewOrder.quantity > 0 && !SellOrders.empty())
    {
        auto BestSell = SellOrders.begin();
        std::uint64_t BestSellPrice = BestSell->first;

        if (NewOrder.priceTicks < BestSellPrice)
        {
            break;
        }

        std::list<Order> &OrdersAtBestSellPrice = BestSell->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestSellPrice.empty())
        {
            Order &BestSellOrder = OrdersAtBestSellPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestSellOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestSellOrder.quantity -= TradeQuantity;

            tradeHistory.AddTrade(symbol, NewOrder.id, BestSellOrder.id, BestSellPrice, TradeQuantity, AggressorSide::Buy);
            std::cout << "TRADE: Buy Order " << NewOrder.id << " matched with Sell Order " << BestSellOrder.id << " | Price: " << FormatPrice(BestSellPrice) << " | Quantity: " << TradeQuantity << " | Order Type: Limit" << std::endl;

            if (BestSellOrder.quantity == 0)
            {
                OrderLocations.erase(BestSellOrder.id);

                OrdersAtBestSellPrice.pop_front();
            }
        }

        if (OrdersAtBestSellPrice.empty())
        {
            SellOrders.erase(BestSell);
        }
    }

    if (NewOrder.quantity > 0)
    {
        std::list<Order> &OrdersAtPrice = BuyOrders[NewOrder.priceTicks];

        OrdersAtPrice.push_back(NewOrder);

        std::list<Order>::iterator NewOrderIterator = std::prev(OrdersAtPrice.end());

        OrderLocations.emplace(NewOrder.id, OrderLocation(NewOrder.side, NewOrder.priceTicks, NewOrderIterator));
    }
}

void OrderBook::HandleLimitSell(Order &NewOrder)
{
    while (NewOrder.quantity > 0 && !BuyOrders.empty())
    {
        auto BestBuy = BuyOrders.begin();
        std::uint64_t BestBuyPrice = BestBuy->first;

        if (NewOrder.priceTicks > BestBuyPrice)
        {
            break;
        }

        std::list<Order> &OrdersAtBestBuyPrice = BestBuy->second;

        while (NewOrder.quantity > 0 && !OrdersAtBestBuyPrice.empty())
        {
            Order &BestBuyOrder = OrdersAtBestBuyPrice.front();

            int TradeQuantity = std::min(NewOrder.quantity, BestBuyOrder.quantity);

            NewOrder.quantity -= TradeQuantity;
            BestBuyOrder.quantity -= TradeQuantity;

            tradeHistory.AddTrade(symbol, BestBuyOrder.id, NewOrder.id, BestBuyPrice, TradeQuantity, AggressorSide::Sell);
            std::cout << "TRADE: Sell Order " << NewOrder.id << " matched with Buy Order " << BestBuyOrder.id << " | Price: " << FormatPrice(BestBuyPrice) << " | Quantity: " << TradeQuantity << " | Order Type: Limit" << std::endl;

            if (BestBuyOrder.quantity == 0)
            {
                OrderLocations.erase(BestBuyOrder.id);

                OrdersAtBestBuyPrice.pop_front();
            }
        }

        if (OrdersAtBestBuyPrice.empty())
        {
            BuyOrders.erase(BestBuy);
        }
    }

    if (NewOrder.quantity > 0)
    {
        std::list<Order> &OrdersAtPrice = SellOrders[NewOrder.priceTicks];

        OrdersAtPrice.push_back(NewOrder);

        std::list<Order>::iterator NewOrderIterator = std::prev(OrdersAtPrice.end());

        OrderLocations.emplace(NewOrder.id, OrderLocation(NewOrder.side, NewOrder.priceTicks, NewOrderIterator));
    }
}

void OrderBook::AddOrder(OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity)
{
    if (quantity <= 0)
    {
        std::cout << "ERROR: Valid Order quantity is required" << std::endl;
        return;
    }

    if (type == OrderType::Limit && priceTicks <= 0)
    {
        std::cout << "ERROR: Valid Order price is required for a Limit Order" << std::endl;
        return;
    }

    Order NewOrder(nextOrderId, nextOrderSequenceNumber, side, type, priceTicks, quantity);

    if (type == OrderType::Market)
    {
        if (side == OrderSide::Buy)
        {
            HandleMarketBuy(NewOrder);
        }
        else
        {
            HandleMarketSell(NewOrder);
        }
    }
    else
    {
        if (side == OrderSide::Buy)
        {
            HandleLimitBuy(NewOrder);
        }
        else
        {
            HandleLimitSell(NewOrder);
        }
    }

    nextOrderId++;
    nextOrderSequenceNumber++;
}

void OrderBook::CancelOrder(std::uint64_t id)
{
    auto Order = OrderLocations.find(id);

    if (Order == OrderLocations.end())
    {
        std::cout << "CANCEL REJECTED: Order " << id << " was not found." << std::endl;
        return;
    }

    OrderLocation &OrderData = Order->second;

    if (OrderData.side == OrderSide::Buy)
    {
        auto OrderLevel = BuyOrders.find(OrderData.priceTicks);

        if (OrderLevel != BuyOrders.end())
        {
            OrderLevel->second.erase(OrderData.orderIterator);

            std::cout << "CANCELLED: Buy Order " << id << " | Price: " << FormatPrice(OrderData.priceTicks) << std::endl;

            if (OrderLevel->second.empty())
            {
                BuyOrders.erase(OrderLevel);
                std::cout << "PRICE LEVEL REMOVED: Buy level " << FormatPrice(OrderData.priceTicks) << " is now empty." << std::endl;
            }
        }
        else
        {
            std::cout << "WARNING: Buy Order " << id << " was found in OrderLocations, but price level " << FormatPrice(OrderData.priceTicks) << " was missing. Cleaning up stale order location." << std::endl;
        }
    }
    else
    {
        auto OrderLevel = SellOrders.find(OrderData.priceTicks);

        if (OrderLevel != SellOrders.end())
        {
            OrderLevel->second.erase(OrderData.orderIterator);

            std::cout << "CANCELLED: Sell Order " << id << " | Price: " << FormatPrice(OrderData.priceTicks) << std::endl;

            if (OrderLevel->second.empty())
            {
                SellOrders.erase(OrderLevel);
                std::cout << "PRICE LEVEL REMOVED: Sell level " << FormatPrice(OrderData.priceTicks) << " is now empty." << std::endl;
            }
        }
        else
        {
            std::cout << "WARNING: Sell Order " << id << " was found in OrderLocations, but price level " << FormatPrice(OrderData.priceTicks) << " was missing. Cleaning up stale order location." << std::endl;
        }
    }

    OrderLocations.erase(id);
}

void OrderBook::PrintOrderBook() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";

    auto printSectionHeader = [&](const std::string &title, const std::string &color)
    {
        std::cout << BOLD << color << "\n============================================================\n"
                  << "  " << title << "\n"
                  << "============================================================\n"
                  << RESET;
    };

    auto printColumnHeader = [&]()
    {
        std::cout << DIM << std::left << std::setw(14) << "  PRICE" << std::setw(10) << "QTY" << std::setw(12) << "ORDER ID" << std::setw(12) << "SEQ" << std::setw(12) << "LVL QTY" << "\n"
                  << "  ----------------------------------------------------------\n"
                  << RESET;
    };

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                       ORDER BOOK                         #\n"
              << "############################################################\n"
              << RESET;

    printSectionHeader("SELL SIDE / ASKS", RED);
    printColumnHeader();

    if (SellOrders.empty())
    {
        std::cout << DIM << "  No sell orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &[price, orders] : SellOrders)
        {
            int levelQty = 0;

            for (const Order &o : orders)
                levelQty += o.quantity;

            bool firstAtLevel = true;

            for (const Order &o : orders)
            {
                std::cout << RED << std::left << std::setw(14) << ("  " + FormatPrice(price)) << RESET << std::setw(10) << o.quantity << std::setw(12) << o.id << std::setw(12) << o.sequenceNumber;

                if (firstAtLevel)
                {
                    std::cout << YELLOW << std::setw(12) << levelQty << RESET;
                    firstAtLevel = false;
                }

                std::cout << "\n";
            }

            std::cout << DIM << "  ----------------------------------------------------------\n"
                      << RESET;
        }
    }

    std::cout << "\n";

    if (!BuyOrders.empty() && !SellOrders.empty())
    {
        std::uint64_t bestBid = BuyOrders.begin()->first;
        std::uint64_t bestAsk = SellOrders.begin()->first;
        std::int64_t spread = static_cast<std::int64_t>(bestAsk) - static_cast<std::int64_t>(bestBid);

        std::cout << BOLD;

        if (spread < 0)
        {
            std::cout << RED << "  WARNING: Book crossed. Spread: -" << FormatPrice(static_cast<std::uint64_t>(-spread));
        }
        else
        {
            std::cout << "  Best bid: " << GREEN << FormatPrice(bestBid) << RESET << BOLD << "  |  Best ask: " << RED << FormatPrice(bestAsk) << RESET << BOLD << "  |  Spread: " << YELLOW << FormatPrice(static_cast<std::uint64_t>(spread));
        }

        std::cout << RESET << "\n";
    }
    else
    {
        std::cout << DIM << "  Spread unavailable - both sides need orders.\n"
                  << RESET;
    }

    printSectionHeader("BUY SIDE / BIDS", GREEN);
    printColumnHeader();

    if (BuyOrders.empty())
    {
        std::cout << DIM << "  No buy orders.\n"
                  << RESET;
    }
    else
    {
        for (const auto &[price, orders] : BuyOrders)
        {
            int levelQty = 0;

            for (const Order &o : orders)
                levelQty += o.quantity;

            bool firstAtLevel = true;

            for (const Order &o : orders)
            {
                std::cout << GREEN << std::left << std::setw(14) << ("  " + FormatPrice(price)) << RESET << std::setw(10) << o.quantity << std::setw(12) << o.id << std::setw(12) << o.sequenceNumber;

                if (firstAtLevel)
                {
                    std::cout << YELLOW << std::setw(12) << levelQty << RESET;
                    firstAtLevel = false;
                }

                std::cout << "\n";
            }

            std::cout << DIM << "  ----------------------------------------------------------\n"
                      << RESET;
        }
    }

    {
        int totalBidLevels = static_cast<int>(BuyOrders.size());
        int totalAskLevels = static_cast<int>(SellOrders.size());
        int totalBidQty = 0;
        int totalAskQty = 0;

        for (const auto &[price, orders] : BuyOrders)
            for (const Order &o : orders)
                totalBidQty += o.quantity;

        for (const auto &[price, orders] : SellOrders)
            for (const Order &o : orders)
                totalAskQty += o.quantity;

        std::cout << DIM << "\n  Bid levels: " << totalBidLevels << "  |  Ask levels: " << totalAskLevels << "  |  Total bid qty: " << totalBidQty << "  |  Total ask qty: " << totalAskQty << "\n"
                  << RESET;
    }

    std::cout << BOLD << CYAN << "############################################################\n"
              << RESET << "\n";
}

void OrderBook::PrintTradeHistory() const
{
    tradeHistory.PrintTradeHistory();
}