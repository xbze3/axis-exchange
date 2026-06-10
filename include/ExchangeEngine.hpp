#pragma once

#include <iostream>
#include <map>
#include <list>
#include <string>

#include "OrderBook.hpp"

class ExchangeEngine
{
    std::map<std::string, OrderBook> OrderBooks;

public:
};