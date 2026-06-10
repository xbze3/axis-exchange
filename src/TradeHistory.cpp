#include <iostream>

#include "TradeHistory.hpp"
#include "Trade.hpp"
#include <iomanip>

TradeHistory::TradeHistory()
{
    nextTradeId = 1;
};

void TradeHistory::AddTrade(const std::string &symbol, std::uint64_t buyOrderId, std::uint64_t sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide)
{
    Trade newTrade(nextTradeId, symbol, buyOrderId, sellOrderId, priceTicks, quantity, aggressorSide);

    trades.push_back(newTrade);

    nextTradeId++;
}

void TradeHistory::PrintTradeHistory() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string GREEN = "\033[32m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";
    const std::string YELLOW = "\033[33m";

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                     TRADE HISTORY                        #\n"
              << "############################################################\n"
              << RESET;

    if (trades.empty())
    {
        std::cout << DIM << "\n  No trades executed yet.\n\n"
                  << RESET;
        std::cout << CYAN << "############################################################\n"
                  << RESET << "\n";
        return;
    }

    int totalQty = 0;
    int buyAggCount = 0;
    int sellAggCount = 0;

    for (const Trade &t : trades)
    {
        totalQty += t.quantity;

        if (t.aggressorSide == AggressorSide::Buy)
        {
            ++buyAggCount;
        }
        else
        {
            ++sellAggCount;
        }
    }

    std::cout << "\n"
              << "  " << std::left << std::setw(28) << "Total trades executed:" << BOLD << trades.size() << RESET << "\n";
    std::cout << "  " << std::left << std::setw(28) << "Total volume:" << BOLD << totalQty << RESET << "\n";
    std::cout << "  " << std::left << std::setw(28) << "Buy-aggressed:" << BOLD << GREEN << buyAggCount << RESET << "\n";
    std::cout << "  " << std::left << std::setw(28) << "Sell-aggressed:" << BOLD << RED << sellAggCount << RESET << "\n";

    std::cout << DIM << "\n  --------------------------------------------------------\n"
              << RESET;

    for (const Trade &t : trades)
    {
        t.PrintTrade();
    }

    std::cout << BOLD << CYAN << "############################################################\n"
              << RESET << "\n";
}