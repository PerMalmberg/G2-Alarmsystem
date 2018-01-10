#pragma once


#include "AlarmBaseState.h"

class Armed
        : public AlarmBaseState
{
    public:
        explicit Armed(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm, "Armed")
        {
        }

        void enter_state() override;
        void leave_state() override;

        void disarm() override;
        void tick() override;

        bool is_armed() const override
        {
            return true;
        }

        void event(const AnalogValueOutsideLimits& event) override;
        void event(const DigitalValueNotIdle& event) override;
    private:
        uint16_t led = 0;
};

