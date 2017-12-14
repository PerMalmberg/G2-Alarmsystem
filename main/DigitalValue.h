#pragma once

class DigitalValue
{
    public:
        DigitalValue() = default;

        DigitalValue(uint8_t input, bool value)
                : input(input),
                  value(value)
        {
        }

        uint8_t get_input() const
        {
            return input;
        }

        bool get_value() const
        {
            return value;
        }

    private:
        uint8_t input = 0;
        bool value = 0;
};
