
#include "Armed.h"
#include "Idle.h"
#include "Tripped.h"
#include <chrono>

using namespace smooth::core::logging;
using namespace std::chrono;

void Armed::enter_state()
{
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
    if(fsm.is_input_enabled(event.get_name()))
    {
        fsm.set_state(new(fsm)Tripped(fsm));
    }
}

void Armed::event(const DigitalValueNotIdle& event)
{
    if(fsm.is_input_enabled(event.get_name()))
    {
        fsm.set_state(new(fsm)Tripped(fsm));
    }
}

void Armed::tick()
{
    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 33, 33, 0);
    fsm.apply_rgb();
}
