#pragma once

#include <stdint.h>

class AnalogValue
{
    public:
        AnalogValue() = default;

        AnalogValue(int input, uint32_t value)
                : input(input),
                  value(value)
        {
        }

        int get_input() const
        {
            return input;
        }

        uint32_t get_value() const
        {
            return value;
        }

    private:
        int input = 0;
        uint32_t value = 0;
};
