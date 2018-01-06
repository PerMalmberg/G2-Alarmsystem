#pragma once

#include <cstdint>

class I2CSetOutput
{
    public:
        I2CSetOutput() = default;

        I2CSetOutput(uint8_t io, bool state)
                : io(io), state(state)
        {
        }

        uint8_t get_io() const
        {
            return io;
        }

        bool get_state() const
        {
            return state;
        }

    private:
        uint8_t io;
        bool state;
};