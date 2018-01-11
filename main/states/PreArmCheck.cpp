#include "PreArmCheck.h"
#include "Idle.h"
#include "Armed.h"

void PreArmCheck::enter_state()
{
    // Check if all inputs are within limits
    if (fsm.are_all_inputs_within_limits())
    {
        // QQQ Signal exit period started
        elapsed.start();
    }
    else
    {
        // QQQ Signal error
        fsm.set_state(new(fsm) Idle(fsm));
    }
}

void PreArmCheck::tick()
{
    if (elapsed.get_running_time() > fsm.get_exit_delay())
    {
        if (fsm.are_all_inputs_within_limits())
        {
            fsm.set_state(new(fsm) Armed(fsm));
        }
        else
        {
            // QQQ Signal error
            fsm.set_state(new(fsm) Idle(fsm));
        }
    }

    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 0, 33, 33);
    fsm.apply_rgb();
}

void PreArmCheck::disarm()
{
    fsm.set_state(new(fsm) Idle(fsm));
}
