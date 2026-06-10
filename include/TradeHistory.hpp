#pragma once

#include <vector>
#include <iostream>

#include "Trade.hpp"

class TradeHistory
{
    std::uint64_t nextTradeId;
    std::vector<Trade> trades;

public:
    TradeHistory();

    void AddTrade(const std::string &symbol, std::uint64_t buyOrderId, std::uint64_t sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide);

    void PrintTradeHistory() const;
};
