#pragma once

#include "AlarmBaseState.h"
#include "AlarmFSM.h"

class Idle
        : public AlarmBaseState
{
    public:
        explicit Idle(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm)
        {

        }

        void enter_state() override;

        void arm() override;
};
