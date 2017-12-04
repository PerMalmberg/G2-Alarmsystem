//
// Created by permal on 12/4/17.
//

#include "Wiegand.h"
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

Wiegand::Wiegand(smooth::core::Task& task)
        : d1_queue(task, *this),
          d2_queue(task, *this),
          d1(d1_queue, GPIO_NUM_0, false, false, GPIO_INTR_NEGEDGE),
          d2(d2_queue, GPIO_NUM_4, false, false, GPIO_INTR_NEGEDGE)
{
}

void Wiegand::event(const smooth::core::io::InterruptInputEvent& event)
{
    Log::info("W", Format("{1}", UInt32(event.get_io())));
}