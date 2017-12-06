//
// Created by permal on 12/4/17.
//

#include "Wiegand.h"
#include <chrono>
#include <string>
#include <smooth/core/logging/log.h>
#include <smooth/core/util/ByteSet.h>

using namespace std::chrono;
using namespace smooth::core::logging;
using namespace smooth::core::timer;
using namespace smooth::core::util;

static const gpio_num_t D0 = GPIO_NUM_0;
static const gpio_num_t D1 = GPIO_NUM_4;
static const std::chrono::milliseconds timeout(25);

Wiegand::Wiegand(smooth::core::Task& task)
        : d0_queue(task, *this),
          d1_queue(task, *this),
          d0(d0_queue, GPIO_NUM_0, false, false, GPIO_INTR_NEGEDGE),
          d1(d1_queue, GPIO_NUM_4, false, false, GPIO_INTR_NEGEDGE),
          line_silent("WiegandLineSilent", 5, task, *this),
          expire(Timer::create("WiegandLineSilent", 1, line_silent, false, milliseconds(25)))
{
}

void Wiegand::event(const smooth::core::io::InterruptInputEvent& event)
{
    expire->start(timeout);
    bit_count++;
    data <<= 1;
    data.set(data.size() - 1, event.get_io() == D1);
    Log::info("event", Format("{1}", UInt32(bit_count)));
}

void Wiegand::event(const smooth::core::timer::TimerExpiredEvent& event)
{
    Log::info("Wiegand", Format("{1}", UInt32(bit_count)));

    bit_count = 0;

    if (bit_count == 8)
    {
        // Keyboard data
        auto high = data.to_ulong() & 0x0F;
        auto low = data.to_ulong() & 0xF0;

        Log::info("QQQ", Format("Low: {1}, High: {2}, ~Low: {3}", UInt32(low), UInt32(high), UInt32(~low)));

        if (high == ~low)
        {

        }

    }


    data.reset();


}