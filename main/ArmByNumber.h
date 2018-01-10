#pragma once

#include "Config.h"
#include <chrono>
#include <smooth/core/timer/ElapsedTime.h>
#include "IAlarmState.h"

class ArmByNumber
{
    public:
        ArmByNumber(Config& cfg, IAlarmState& alarm_state, int max_numbers_per_code, int reset_number, int commit_number, std::chrono::milliseconds timeout)
                : cfg(cfg),
                  alarm_state(alarm_state),
                  max_numbers_per_code(max_numbers_per_code),
                  reset_number(reset_number),
                  commit_number(commit_number),
                  timeout_interval(timeout)
        {
        }

        void tick();
        void number(int num);

        void commit();
    private:
        std::string current{};
        Config& cfg;
        IAlarmState& alarm_state;
        int max_numbers_per_code;
        int reset_number;
        int commit_number;
        std::chrono::milliseconds timeout_interval;
        smooth::core::timer::ElapsedTime timeout{};
};