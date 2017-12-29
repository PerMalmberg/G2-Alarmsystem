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

        virtual void tick()
        {
        }

        virtual void event(const AnalogValueOutsideLimits& event)
        {
        }

        virtual void event(const DigitalValueNotIdle& event)
        {
        }

    protected:
        AlarmFSM<AlarmBaseState>& fsm;
};