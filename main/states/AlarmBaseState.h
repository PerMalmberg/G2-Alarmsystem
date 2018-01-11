#pragma once

#include "AlarmFSM.h"

class AlarmBaseState
{
    public:
        explicit AlarmBaseState(AlarmFSM<AlarmBaseState>& fsm, const std::string& name) : fsm(fsm), name(name)
        {
        }

        virtual void enter_state()
        {
        }

        virtual void leave_state()
        {
        }

        virtual void arm()
        {
        }

        virtual void disarm()
        {
        }

        virtual void tick()
        {
        }

        virtual bool is_armed() const
        {
            return false;
        }

        virtual bool is_arming() const
        {
            return false;
        }

        std::string get_name() const
        {
            return name;
        }

        virtual void event(const AnalogValueOutsideLimits& event)
        {
        }

        virtual void event(const DigitalValueNotIdle& event)
        {
        }

    protected:
        AlarmFSM<AlarmBaseState>& fsm;
    private:
        std::string name;
};