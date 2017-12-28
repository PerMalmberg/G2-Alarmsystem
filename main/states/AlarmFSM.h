#pragma once

#include <smooth/core/fsm/StaticFSM.h>
#include "IOStatus.h"

template<typename BaseState>
class AlarmFSM : public smooth::core::fsm::StaticFSM<BaseState, 100>
{
    public:
        explicit AlarmFSM(IOStatus& io_status);

        void arm();
        bool are_all_inputs_within_limits() const;
        void disarm();
    private:
        IOStatus& io_status;
};

template<typename BaseState>
void AlarmFSM<BaseState>::arm()
{
    this->get_state()->arm();
}

template<typename BaseState>
AlarmFSM<BaseState>::AlarmFSM(IOStatus& io_status)
        : io_status(io_status)
{
}

template<typename BaseState>
bool AlarmFSM<BaseState>::are_all_inputs_within_limits() const
{
    return io_status.are_all_inputs_within_limits();
}

template<typename BaseState>
void AlarmFSM<BaseState>::disarm()
{
    this->get_state()->disarm();
}

