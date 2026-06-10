#pragma once

#include <iostream>
#include <cstdint>

class InstrumentId
{
public:
    std::uint64_t value;

    InstrumentId(std::uint64_t value);

    bool operator<(const InstrumentId &other) const;
};