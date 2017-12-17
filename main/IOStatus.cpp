#include "IOStatus.h"

using namespace smooth::core::logging;

void IOStatus::event(const AnalogValue& event)
{
    analog_status[event.get_input()] = event.get_value();

    if(!compare_diff(analog_status_reference, analog_status, 100))
    {
        //Log::info("Analog", Format(""));
    }
}

void IOStatus::event(const DigitalValue& event)
{
    digital_status[event.get_input()] = event.get_value();

    if(!compare_equal(digital_status_reference, digital_status))
    {
        //Log::info("Digital", Format(""));
    }
}

void IOStatus::arm()
{
    // Store current reference values
    analog_status_reference = analog_status;
    digital_status_reference = digital_status;
}
