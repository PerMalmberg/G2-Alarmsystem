#include "Idle.h"
#include "PreArmCheck.h"

void Idle::arm()
{
    fsm.set_state(new(fsm) PreArmCheck(fsm));
}

void Idle::enter_state()
{
    fsm.clear_rgb();
    fsm.apply_rgb();
}

void Idle::tick()
{
    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 0, 33, 0);
    fsm.apply_rgb();
}