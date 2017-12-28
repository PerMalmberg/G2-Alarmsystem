#pragma once

#include "AlarmFSM.h"

class AlarmBaseState
{
    public:
        explicit AlarmBaseState(AlarmFSM<AlarmBaseState>& fsm) : fsm(fsm)
        {
        }

        virtual void enter_state() {}
        virtual void leave_state() {}

        virtual void arm()
        {
        }

        virtual void disarm()
        {
        }

    protected:
        AlarmFSM<AlarmBaseState>& fsm;
};