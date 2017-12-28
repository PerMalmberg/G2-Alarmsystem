#pragma once


#include "AlarmBaseState.h"

class Armed
        : public AlarmBaseState
{
    public:
        explicit Armed(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm)
        {

        }

        void enter_state() override;

        void disarm() override;
};

