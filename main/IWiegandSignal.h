#pragma once

class IWiegandSignal
{
    public:
        virtual void number(uint8_t num) = 0;
        virtual void id(uint32_t id, uint8_t byte_count) = 0;
};
