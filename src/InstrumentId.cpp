#include <iostream>
#include <cstdint>

#include "InstrumentId.hpp"

InstrumentId::InstrumentId(std::uint64_t value) : value(value)
{
}

bool InstrumentId::operator<(const InstrumentId &other) const
{
    return value < other.value;
}