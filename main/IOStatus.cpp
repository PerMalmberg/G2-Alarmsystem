#include "IOStatus.h"

using namespace smooth::core::logging;

void IOStatus::event(const AnalogValue& event)
{
    std::stringstream ss;
    ss << "a" << event.get_input();

    analog_values[ss.str()] = event.get_value();

    auto ref = ref_getter.get_analog_reference(ss.str());
    auto max = ref.value + ref.variance;
    auto min = ref.value - ref.variance;
    if (event.get_value() > max || event.get_input() < min)
    {
        Log::warning("IOStatus", Format("Analog {1}", Int32(event.get_input())));
    }
}

void IOStatus::event(const DigitalValue& event)
{
    std::stringstream ss;
    ss << "i" << event.get_input();
    digital_values[ss.str()] = event.get_value();
}


