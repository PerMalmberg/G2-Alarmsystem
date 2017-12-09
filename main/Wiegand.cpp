//
// Created by permal on 12/4/17.
//

#include "Wiegand.h"
#include <chrono>
#include <string>
#include <bitset>
#include <smooth/core/logging/log.h>


using namespace std::chrono;
using namespace smooth::core::logging;
using namespace smooth::core::timer;

static const gpio_num_t D0 = GPIO_NUM_0;
static const gpio_num_t D1 = GPIO_NUM_4;
static const std::chrono::milliseconds timeout(25);

Wiegand::Wiegand(smooth::core::Task& task, IWiegandSignal& receiver)
        : receiver(receiver),
          bit_queue(task, *this),
          d0(bit_queue, GPIO_NUM_0, false, false, GPIO_INTR_NEGEDGE),
          d1(bit_queue, GPIO_NUM_4, false, false, GPIO_INTR_NEGEDGE),
          line_silent("WiegandLineSilent", 5, task, *this),
          expire(Timer::create("WiegandLineSilent", 1, line_silent, false, milliseconds(25)))
{
}

void Wiegand::event(const smooth::core::io::InterruptInputEvent& event)
{
    expire->start(timeout);
    ++bit_count;
    data <<= 1;
    data.set(0, event.get_io() == D1);
}

void Wiegand::event(const smooth::core::timer::TimerExpiredEvent& event)
{
    if (bit_count == 4)
    {
        // No integrity data
        receiver.number(static_cast<uint8_t>(data.to_ulong() & 0xF));
    }
    else if (bit_count == 8)
    {
        // Integrity via high nibble = ~(low nibble), low nibble being actual data.
        auto low = data.to_ulong() & 0x0F;
        auto high = data.to_ulong() & 0xF0;
        if (low == ~high)
        {
            receiver.number(low);
        }
    }
    else if (bit_count == 26)
    {
        // Integrity data: MSB; even for 12 upper data bits, and LSB; odd for lower 12 data bits
        std::bitset<13> upper(data.to_ulong() >> 13);
        std::bitset<13> lower(data.to_ulong() & 0x1FFF);

        // Validate it is even parity on upper part
        bool valid = (upper.count() & 1) == 0;
        // And odd parity on lower part
        valid &= lower.count() & 1;

        if (valid)
        {
            uint32_t id = (upper.to_ulong() & 0xFFF) | (lower.to_ulong() >> 1);
            receiver.id(id, 3);
        }
    }

    bit_count = 0;
    data.reset();
}