#pragma once

#include "AlarmBaseState.h"
#include "AlarmFSM.h"

class Idle
        : public AlarmBaseState
{
    public:
        explicit Idle(AlarmFSM<AlarmBaseState>& fsm) : AlarmBaseState(fsm, "Idle")
        {

        }

        void enter_state() override;

        void arm() override;
        void tick() override ;
    private:
        uint16_t led = 0;
};
