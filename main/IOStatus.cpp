#include "IOStatus.h"

using namespace smooth::core::logging;

bool IOStatus::event(const AnalogValue& event)
{
    std::stringstream ss;
    ss << "a" << event.get_input();
    std::string name = ss.str();

    auto old = analog_values[name];
    analog_values[ss.str()] = event.get_value();

    bool changed = false;

    auto ref = ref_getter.get_analog_reference(name);
    auto max = ref.value + ref.variance;
    auto min = ref.value - ref.variance;

    if (ref_getter.is_input_enabled(name))
    {
        changed = old != analog_values[name];
        if (event.get_value() > max || event.get_value() < min)
        {
            Log::warning("IOStatus", Format("Analog {1} outside range: {3} <-- {2} --> {4}",
                                            Int32(event.get_input()),
                                            UInt32(event.get_value()),
                                            Int32(min),
                                            Int32(max)));
        }
    }

    return changed;
}

bool IOStatus::event(const DigitalValue& event)
{
    std::stringstream ss;
    ss << "i" << static_cast<int>(event.get_input());
    std::string name = ss.str();

    auto old = digital_values[name];
    digital_values[name] = event.get_value();
    bool changed = false;

    if (ref_getter.is_input_enabled(name))
    {
        changed = old != digital_values[name];
        if (digital_values[name] != ref_getter.get_digital_idle(name))
        {
            Log::warning("IOStatus", Format("Digital {1} not idle. Current: {2}, Idle: {3}",
                                            UInt32(event.get_input()),
                                            Bool(event.get_value()),
                                            Bool(ref_getter.get_digital_idle(name))));
        }
    }

    return changed;
}


