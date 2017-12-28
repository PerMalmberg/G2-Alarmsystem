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

    if (ref_getter.is_input_enabled(name))
    {
        changed = old != analog_values[name];
        if (!is_analog_inside_limit(name, event.get_value()))
        {

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
        if (!is_digital_idle(name, event.get_value()) )
        {

        }
    }

    return changed;
}

bool IOStatus::are_all_inputs_within_limits()
{
    auto all_inside = true;

    for (const auto& name : ref_getter.get_analog_input_names())
    {
        if (ref_getter.is_input_enabled(name))
        {
            all_inside &= is_analog_inside_limit(name, analog_values[name]);
        }
    }

    for(const auto& name:ref_getter.get_digital_input_names())
    {
        if(ref_getter.is_input_enabled(name))
        {
            all_inside &= !is_digital_idle(name, digital_values[name]);
        }
    }

    return all_inside;
}

bool IOStatus::is_analog_inside_limit(const std::string& name, uint32_t value)
{
    auto ref = ref_getter.get_analog_reference(name);
    auto max = ref.value + ref.variance;
    auto min = ref.value - ref.variance;

    bool inside = value <= max && value >= min;

    if (!inside)
    {
        Log::warning("IOStatus", Format("Analog {1} outside range: {3} <-- {2} --> {4}",
                                        Str(name),
                                        UInt32(value),
                                        Int32(min),
                                        Int32(max)));
    }

    return inside;
}

bool IOStatus::is_digital_idle(const std::string& name, bool value)
{
    bool is_idle = value == ref_getter.get_digital_idle(name);

    if(!is_idle)
    {
        Log::warning("IOStatus", Format("Digital {1} not idle. Current: {2}, Idle: {3}",
                                        Str(name),
                                        Bool(value),
                                        Bool(ref_getter.get_digital_idle(name))));
    }

    return is_idle;
}


