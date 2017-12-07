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
        // Keyboard data
        receiver.number(static_cast<uint8_t>(data.to_ulong() & 0xF));
    }


    bit_count = 0;
    data.reset();


}