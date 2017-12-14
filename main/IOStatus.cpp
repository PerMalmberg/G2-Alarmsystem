#include "IOStatus.h"
#include <smooth/core/logging/log.h>

using namespace smooth::core::logging;

IOStatus::IOStatus()
{

}

void IOStatus::event(const AnalogValue& event)
{
    analog_status_prev = analog_status;
    analog_status[event.get_input()] = event.get_value();

    if(!compare_diff(analog_status_prev, analog_status, 100))
    {
        Log::info("Analog", Format(""));
    }
}

void IOStatus::event(const DigitalValue& event)
{
    digital_status_prev = digital_status;
    digital_status[event.get_input()] = event.get_value();

    if(!compare_equal(digital_status_prev, digital_status))
    {
        Log::info("Digital", Format(""));
    }
}