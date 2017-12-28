
#include "Armed.h"
#include "Idle.h"

using namespace smooth::core::logging;

void Armed::enter_state()
{
    Log::info("QQQQ", Format("Armed"));
}

void Armed::disarm()
{
    fsm.set_state(new(fsm)Idle(fsm));
}
