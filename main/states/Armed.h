#pragma once


#include "AlarmBaseState.h"
#include <smooth/core/timer/ElapsedTime.h>

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
        smooth::core::timer::ElapsedTime entry_delay{};
        std::chrono::seconds entry_delay_limit{};

        void trip(const std::string& input_name);
};

