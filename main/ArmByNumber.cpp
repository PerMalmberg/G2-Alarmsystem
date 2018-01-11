#include "ArmByNumber.h"

void ArmByNumber::tick()
{
    if (timeout.get_running_time() > timeout_interval)
    {
        current.clear();
    }
}

void ArmByNumber::number(int num)
{
    timeout.reset();

    if (num == reset_number)
    {
        current.clear();
    }
    else if (num == commit_number)
    {
        if (cfg.has_zone_with_code(current))
        {
            if (alarm_state.is_armed() || alarm_state.is_arming())
            {
                // Any valid code disarms
                alarm_state.disarm();
            }
            else
            {
                auto zone = cfg.get_zone_for_code(current);
                alarm_state.arm(zone);
            }

        }

        current.clear();
    }
    else
    {
        current += std::to_string(num);
    }
}

void ArmByNumber::commit()
{
    number(commit_number);
}
