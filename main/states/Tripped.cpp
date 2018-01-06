//
// Created by permal on 12/28/17.
//

#include <I2CSetOutputCmd.h>
#include <smooth/core/ipc/Publisher.h>
#include <I2CSetOutputCmd.h>
#include "Tripped.h"
#include "Idle.h"

using namespace smooth::core::ipc;

void Tripped::enter_state()
{
    I2CSetOutput s(0, true);
    Publisher<I2CSetOutput>::publish(s);
}

void Tripped::leave_state()
{
    I2CSetOutput s(0, false);
    Publisher<I2CSetOutput>::publish(s);
}

void Tripped::disarm()
{
    fsm.set_state(new(fsm)Idle(fsm));
}

void Tripped::tick()
{
    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 33, 0, 0);
    fsm.apply_rgb();
}
