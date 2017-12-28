//
// Created by permal on 12/28/17.
//

#include "PreArmCheck.h"
#include "Armed.h"
#include "Idle.h"

void PreArmCheck::enter_state()
{
    Log::info("QQQQ", Format("PreArmCheck"));

    // Check if all inputs are within limits
    if(fsm.are_all_inputs_within_limits())
    {
        fsm.set_state(new(fsm) Armed(fsm));
    }
    else
    {
        fsm.set_state(new(fsm) Idle(fsm));
    }
}
