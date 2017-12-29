
#include "Armed.h"
#include "Idle.h"
#include "Tripped.h"

using namespace smooth::core::logging;

void Armed::enter_state()
{
    fsm.clear_rgb();

    fsm.set_pixel(3, 5, 5, 5);
    fsm.apply_rgb();
}

void Armed::leave_state()
{
    fsm.clear_rgb();
    fsm.apply_rgb();
}

void Armed::disarm()
{
    fsm.set_state(new(fsm)Idle(fsm));
}

void Armed::event(const AnalogValueOutsideLimits& event)
{
    fsm.set_state(new(fsm)Tripped(fsm));
}

void Armed::event(const DigitalValueNotIdle& event)
{
    fsm.set_state(new(fsm)Tripped(fsm));
}
