#pragma once

#include <chrono>
#include <iostream>
#include <string>

#include "Enums.hpp"

class Trade
{
public:
    std::uint64_t id;
    std::string symbol;
    std::uint64_t buyOrderId;
    std::uint64_t sellOrderId;
    std::uint64_t priceTicks;
    int quantity;
    AggressorSide aggressorSide;
    std::chrono::system_clock::time_point timestamp;

    Trade(std::uint64_t id, std::string symbol, std::uint64_t buyOrderId, std::uint64_t sellOrderId, std::uint64_t priceTicks, int quantity, AggressorSide aggressorSide);

    void PrintTrade() const;
};
