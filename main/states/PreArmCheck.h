#pragma once

#include "AlarmBaseState.h"
#include <smooth/core/timer/ElapsedTime.h>

class PreArmCheck
        : public AlarmBaseState
{
    public:
        explicit PreArmCheck(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm, "PreArmCheck")
        {
        }

        void enter_state() override;

        void tick() override;

        bool is_arming() const override
        {
            return true;
        }

        void disarm() override;

    private:
        smooth::core::timer::ElapsedTime elapsed{};
        uint16_t led = 0;

};

