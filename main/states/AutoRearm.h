#pragma once

#include "AlarmBaseState.h"

class AutoRearm : public AlarmBaseState
{
    public:
        explicit AutoRearm(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm, "AutoRearm")
        {

        }
};
