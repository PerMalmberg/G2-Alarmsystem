#pragma once

#include "DigitalValue.h"

class DigitalValueNotIdle
        : public DigitalValue
{
    public:
        DigitalValueNotIdle() = default;

        DigitalValueNotIdle(uint8_t input, bool value)
                : DigitalValue(input, value)
        {
        }
};