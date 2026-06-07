#include <iostream>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstdint>

#include "OrderBook.hpp"
#include "TradeHistory.hpp"

void ClearScreen()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void Pause()
{
    std::cout << "\nPress Enter to continue...";
    std::cin.ignore();
    std::cin.get();
}

std::uint64_t ParsePriceToTicks(const std::string &priceInput)
{
    std::string dollarsPart;
    std::string centsPart;

    size_t pointPosition = priceInput.find('.');

    if (pointPosition == std::string::npos)
    {
        dollarsPart = priceInput;
        centsPart = "00";
    }
    else
    {
        dollarsPart = priceInput.substr(0, pointPosition);
        centsPart = priceInput.substr(pointPosition + 1);
    }

    if (dollarsPart.empty())
    {
        dollarsPart = "0";
    }

    if (centsPart.empty())
    {
        centsPart = "00";
    }
    else if (centsPart.length() == 1)
    {
        centsPart += "0";
    }
    else if (centsPart.length() > 2)
    {
        centsPart = centsPart.substr(0, 2);
    }

    std::uint64_t dollars = std::stoull(dollarsPart);
    std::uint64_t cents = std::stoull(centsPart);

    return (dollars * 100) + cents;
}

std::string FormatPrice(std::uint64_t priceTicks)
{
    std::uint64_t dollars = priceTicks / 100;
    std::uint64_t cents = priceTicks % 100;

    std::ostringstream output;

    output << "$" << dollars << "."
           << std::setw(2) << std::setfill('0') << cents;

    return output.str();
}

int main()
{
    TradeHistory history;
    OrderBook book(history);

    bool running = true;

    while (running)
    {
        std::cout << "\n================ C++ ORDERBOOK ENGINE ================\n";
        std::cout << "1. Place Order\n";
        std::cout << "2. View Order Book\n";
        std::cout << "3. View Trade History\n";
        std::cout << "4. Exit\n";
        std::cout << "======================================================\n";
        std::cout << "Select an option: ";

        int menuChoice;
        std::cin >> menuChoice;

        if (menuChoice == 1)
        {
            int sideChoice;
            int typeChoice;
            int quantity;
            std::uint64_t priceTicks = 0;

            std::cout << "\nSelect Order Side:\n";
            std::cout << "1. Buy\n";
            std::cout << "2. Sell\n";
            std::cout << "Choice: ";
            std::cin >> sideChoice;

            OrderSide side;

            if (sideChoice == 1)
            {
                side = OrderSide::Buy;
            }
            else if (sideChoice == 2)
            {
                side = OrderSide::Sell;
            }
            else
            {
                std::cout << "Invalid order side.\n";
                Pause();
                ClearScreen();
                continue;
            }

            std::cout << "\nSelect Order Type:\n";
            std::cout << "1. Limit\n";
            std::cout << "2. Market\n";
            std::cout << "Choice: ";
            std::cin >> typeChoice;

            OrderType type;

            if (typeChoice == 1)
            {
                type = OrderType::Limit;
            }
            else if (typeChoice == 2)
            {
                type = OrderType::Market;
            }
            else
            {
                std::cout << "Invalid order type.\n";
                Pause();
                ClearScreen();
                continue;
            }

            std::cout << "\nEnter Quantity: ";
            std::cin >> quantity;

            if (type == OrderType::Limit)
            {
                std::string priceInput;

                std::cout << "Enter Limit Price: ";
                std::cin >> priceInput;

                priceTicks = ParsePriceToTicks(priceInput);
            }

            ClearScreen();

            std::cout << "\n================ ORDER SUBMITTED ================\n";
            std::cout << "Side: " << (side == OrderSide::Buy ? "Buy" : "Sell") << std::endl;
            std::cout << "Type: " << (type == OrderType::Limit ? "Limit" : "Market") << std::endl;
            std::cout << "Quantity: " << quantity << std::endl;

            if (type == OrderType::Limit)
            {
                std::cout << "Price: " << FormatPrice(priceTicks) << std::endl;
            }

            std::cout << "=================================================\n\n";

            book.AddOrder(side, type, priceTicks, quantity);

            book.PrintOrderBook();
            history.PrintTradeHistory();

            Pause();
            ClearScreen();
        }
        else if (menuChoice == 2)
        {
            ClearScreen();
            book.PrintOrderBook();
            Pause();
            ClearScreen();
        }
        else if (menuChoice == 3)
        {
            ClearScreen();
            history.PrintTradeHistory();
            Pause();
            ClearScreen();
        }
        else if (menuChoice == 4)
        {
            running = false;
        }
        else
        {
            std::cout << "Invalid menu option.\n";
            Pause();
            ClearScreen();
        }
    }

    std::cout << "\nExiting C++ OrderBook Engine...\n";

    return 0;
}