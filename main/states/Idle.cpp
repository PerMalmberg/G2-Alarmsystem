#include "Idle.h"
#include "PreArmCheck.h"

void Idle::arm()
{
    fsm.set_state(new(fsm) PreArmCheck(fsm));
}

void Idle::enter_state()
{
    Log::info("QQQQ", Format("Idle"));
}
