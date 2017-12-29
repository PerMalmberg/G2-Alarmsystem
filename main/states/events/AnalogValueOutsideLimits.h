#pragma once

#include "AnalogValue.h"

class AnalogValueOutsideLimits
        : public AnalogValue
{
    public:
        AnalogValueOutsideLimits() = default;

        AnalogValueOutsideLimits(int input, uint32_t value)
                : AnalogValue(input, value)
        {
        }
};
