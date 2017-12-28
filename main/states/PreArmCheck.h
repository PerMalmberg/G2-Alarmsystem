#pragma once

#include "AlarmBaseState.h"

class PreArmCheck
        : public AlarmBaseState
{
    public:
        explicit PreArmCheck(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm)
        {
        }

        void enter_state() override;
};

