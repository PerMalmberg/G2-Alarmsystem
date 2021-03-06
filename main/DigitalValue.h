#pragma once

#include <string>

class DigitalValue
{
    public:
        DigitalValue() = default;

        DigitalValue(uint8_t input, bool value)
                : input(input),
                  value(value),
                  name("i" + std::to_string(input))
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

        const std::string& get_name() const
        {
            return name;
        }

    private:
        uint8_t input = 0;
        bool value = false;
        std::string name;
};
