
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
    trip(event.get_name());
}

void Armed::event(const DigitalValueNotIdle& event)
{
    trip(event.get_name());
}

void Armed::tick()
{
    fsm.clear_rgb();
    if (entry_delay.is_running())
    {
        fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 33, 33, 33);
    }
    else
    {
        fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 33, 33, 0);
    }

    fsm.apply_rgb();

    if (entry_delay.get_running_time() > entry_delay_limit)
    {
        fsm.set_state(new(fsm)Tripped(fsm));
    }
}

void Armed::trip(const std::string& input_name)
{
    if (!entry_delay.is_running())
    {
        if (fsm.is_input_enabled(input_name))
        {
            entry_delay_limit = fsm.get_entry_delay(input_name);
            entry_delay.start();
        }
    }
}
