#pragma once


#include "AlarmBaseState.h"

class Tripped
        : public AlarmBaseState
{
    public:
        explicit Tripped(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm, "Tripped")
        {
        }

        void disarm() override;
        void tick() override;

        void enter_state() override;
        void leave_state() override;

        bool is_armed() const override
        {
            return true;
        }

    private:
        uint16_t led = 0;
};

