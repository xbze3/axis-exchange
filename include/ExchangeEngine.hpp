#pragma once

#include <iostream>
#include <map>
#include <list>
#include <string>

#include "OrderBook.hpp"
#include "InstrumentId.hpp"
#include "SymbolRegistry.hpp"

class ExchangeEngine
{
private:
    SymbolRegistry registry;
    std::map<InstrumentId, OrderBook> OrderBooks;

    OrderBook *GetBook(const std::string &symbol);

public:
    ExchangeEngine();

    bool AddBook(const std::string &symbol);
    bool RemoveBook(const std::string &symbol);

    bool SubmitOrder(const std::string &symbol, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity);
    bool CancelOrder(const std::string &symbol, std::uint64_t orderId);

    bool HasBook(const std::string &symbol) const;
    const OrderBook *GetBook(const std::string &symbol) const;

    void PrintBook(const std::string &symbol) const;
    void PrintAllBooks() const;
};