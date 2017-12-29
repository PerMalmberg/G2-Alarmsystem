#pragma once


#include "AlarmBaseState.h"

class Tripped
        : public AlarmBaseState
{
    public:
        explicit Tripped(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm)
        {

        }

        void enter_state() override;
};

