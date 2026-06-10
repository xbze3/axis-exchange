#pragma once

#include <iostream>
#include <unordered_map>
#include <cstdint>
#include <string>

#include "InstrumentId.hpp"

class SymbolRegistry
{
    std::unordered_map<std::string, InstrumentId> symbolToInstrumentId;
    std::unordered_map<InstrumentId, std::string> instrumentIdToSymbol;

    std::string NormalizeSymbol(const std::string &symbol) const;

    std::uint64_t nextInstrumentId;

public:
    SymbolRegistry();

    bool RegisterInstrument(const std::string &symbol);
    bool HasSymbol(const std::string &symbol) const;
    bool TryGetInstrumentId(const std::string &symbol, InstrumentId &instrumentId) const;
    bool TryGetSymbol(const InstrumentId &instrumentId, std::string &symbol) const;

};