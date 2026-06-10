#include <iostream>

#include "ExchangeEngine.hpp"

ExchangeEngine::ExchangeEngine()
{
}

bool ExchangeEngine::AddBook(const std::string &symbol)
{
}

bool ExchangeEngine::RemoveBook(const std::string &symbol)
{
}

bool ExchangeEngine::SubmitOrder(const std::string &symbol, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity)
{
}

bool ExchangeEngine::CancelOrder(const std::string &symbol, std::uint64_t orderId)
{
}

bool ExchangeEngine::HasBook(const std::string &symbol) const
{
}

OrderBook *ExchangeEngine::GetBook(const std::string &symbol)
{
}

const OrderBook *ExchangeEngine::GetBook(const std::string &symbol) const
{
}

void ExchangeEngine::PrintBook(const std::string &symbol) const
{
}

void ExchangeEngine::PrintAllBooks() const
{
}