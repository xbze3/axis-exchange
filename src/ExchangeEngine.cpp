#include <iostream>
#include <iomanip>
#include <iostream>

#include "ExchangeEngine.hpp"
#include "SymbolRegistry.hpp"
#include "OrderBook.hpp"

ExchangeEngine::ExchangeEngine()
{
}

bool ExchangeEngine::AddBook(const std::string &symbol)
{
    if (!registry.RegisterInstrument(symbol))
    {
        return false;
    }

    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "BOOK ADD FAILED: Could not find instrument ID for symbol " << symbol << "." << std::endl;
        return false;
    }

    OrderBooks.try_emplace(id, symbol);

    std::cout << "BOOK ADDED: Symbol " << symbol << " | Instrument ID: " << id.value << std::endl;

    return true;
}

bool ExchangeEngine::RemoveBook(const std::string &symbol)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "BOOK REMOVE REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "BOOK REMOVE REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    OrderBooks.erase(id);

    if (!registry.UnregisterInstrument(symbol))
    {
        std::cout << "BOOK REMOVE WARNING: Book was removed, but symbol " << symbol << " could not be unregistered." << std::endl;
        return false;
    }

    std::cout << "BOOK REMOVED: Symbol " << symbol << " | Instrument ID: " << id.value << std::endl;

    return true;
}

bool ExchangeEngine::SubmitOrder(const std::string &symbol, OrderSide side, OrderType type, std::uint64_t priceTicks, int quantity)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "ORDER REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "ORDER REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    book->second.AddOrder(side, type, priceTicks, quantity);

    return true;
}

bool ExchangeEngine::CancelOrder(const std::string &symbol, std::uint64_t orderId)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << "CANCEL REJECTED: Symbol " << symbol << " is not registered." << std::endl;
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << "CANCEL REJECTED: No order book found for symbol " << symbol << "." << std::endl;
        return false;
    }

    book->second.CancelOrder(orderId);

    return true;
}

bool ExchangeEngine::HasBook(const std::string &symbol) const
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        return false;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        return false;
    }

    return true;
}

OrderBook *ExchangeEngine::GetBook(const std::string &symbol)
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        return nullptr;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        return nullptr;
    }

    return &book->second;
}

const OrderBook *ExchangeEngine::GetBook(const std::string &symbol) const
{
    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        return nullptr;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        return nullptr;
    }

    return &book->second;
}

void ExchangeEngine::PrintBook(const std::string &symbol) const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string CYAN = "\033[36m";
    const std::string RED = "\033[31m";

    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << RED << "PRINT REJECTED: Symbol " << symbol << " is not registered." << RESET << std::endl;
        return;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << RED << "PRINT REJECTED: No order book found for symbol " << symbol << "." << RESET << std::endl;
        return;
    }

    std::string normalizedSymbol;

    if (!registry.TryGetSymbol(id, normalizedSymbol))
    {
        normalizedSymbol = symbol;
    }

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                     EXCHANGE BOOK                        #\n"
              << "############################################################\n"
              << RESET;

    std::cout << "  " << std::left << std::setw(20) << "Symbol:" << BOLD << normalizedSymbol << RESET << "\n";
    std::cout << "  " << std::left << std::setw(20) << "Instrument ID:" << BOLD << id.value << RESET << "\n";

    std::cout << DIM << "  ----------------------------------------------------------\n"
              << RESET;

    book->second.PrintOrderBook();
}

void ExchangeEngine::PrintTradeHistory(const std::string &symbol) const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string RED = "\033[31m";
    const std::string CYAN = "\033[36m";

    InstrumentId id(0);

    if (!registry.TryGetInstrumentId(symbol, id))
    {
        std::cout << RED << "TRADE HISTORY PRINT REJECTED: Symbol " << symbol << " is not registered." << RESET << std::endl;
        return;
    }

    auto book = OrderBooks.find(id);

    if (book == OrderBooks.end())
    {
        std::cout << RED << "TRADE HISTORY PRINT REJECTED: No order book found for symbol " << symbol << "." << RESET << std::endl;
        return;
    }

    std::string normalizedSymbol;

    if (!registry.TryGetSymbol(id, normalizedSymbol))
    {
        normalizedSymbol = symbol;
    }

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                 EXCHANGE TRADE HISTORY                  #\n"
              << "############################################################\n"
              << RESET;

    std::cout << "  Symbol: " << BOLD << normalizedSymbol << RESET
              << "  |  Instrument ID: " << BOLD << id.value << RESET << "\n";

    book->second.PrintTradeHistory();
}

void ExchangeEngine::PrintAllBooks() const
{
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string CYAN = "\033[36m";

    std::cout << BOLD << CYAN << "\n############################################################\n"
              << "#                    EXCHANGE BOOKS                        #\n"
              << "############################################################\n"
              << RESET;

    if (OrderBooks.empty())
    {
        std::cout << DIM << "\n  No order books available.\n\n"
                  << RESET;

        std::cout << CYAN << "############################################################\n"
                  << RESET << "\n";

        return;
    }

    std::cout << "\n"
              << "  " << std::left << std::setw(20) << "Total books:" << BOLD << OrderBooks.size() << RESET << "\n";

    std::cout << DIM << "  ----------------------------------------------------------\n"
              << RESET;

    for (const auto &[instrumentId, orderBook] : OrderBooks)
    {
        std::string symbol;

        if (!registry.TryGetSymbol(instrumentId, symbol))
        {
            symbol = "UNKNOWN";
        }

        std::cout << BOLD << CYAN << "\n============================================================\n"
                  << "  SYMBOL: " << symbol << "  |  INSTRUMENT ID: " << instrumentId.value << "\n"
                  << "============================================================\n"
                  << RESET;

        orderBook.PrintOrderBook();
    }

    std::cout << BOLD << CYAN << "############################################################\n"
              << RESET << "\n";
}