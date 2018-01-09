#include "IOStatus.h"
#include <smooth/core/ipc/Publisher.h>
#include "states/events/AnalogValueOutsideLimits.h"
#include "states/events/DigitalValueNotIdle.h"

using namespace smooth::core::logging;
using namespace smooth::core::ipc;

bool IOStatus::event(const AnalogValue& event)
{
    const auto& name = event.get_name();
    auto old = analog_values[name];
    analog_values[name] = event.get_value();

    bool changed = false;

    if (ref_getter.is_input_enabled(name))
    {
        changed = old != analog_values[name];
        if (!is_analog_inside_limit(name, event.get_value()))
        {
            AnalogValueOutsideLimits a(event.get_input(), event.get_value());
            Publisher<AnalogValueOutsideLimits>::publish(a);
        }
    }

    return changed;
}

bool IOStatus::event(const DigitalValue& event)
{
    const auto& name = event.get_name();

    auto old = digital_values[name];
    digital_values[name] = event.get_value();
    bool changed = false;

    if (ref_getter.is_input_enabled(name))
    {
        changed = old != digital_values[name];
        if (!is_digital_idle(name, event.get_value()) )
        {
            DigitalValueNotIdle d(event.get_input(), event.get_value());
            Publisher<DigitalValueNotIdle>::publish(d);
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
            all_inside &= is_digital_idle(name, digital_values[name]);
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

    if (inside)
    {
        Log::info("IOStatus", Format("Analog {1} inside range: {3} <-- {2} --> {4}",
                                        Str(name),
                                        UInt32(value),
                                        Int32(min),
                                        Int32(max)));
    }
    else
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

    if(is_idle)
    {
        Log::info("IOStatus", Format("Digital {1} idle. Current: {2}, Idle: {3}",
                                        Str(name),
                                        Bool(value),
                                        Bool(ref_getter.get_digital_idle(name))));
    }
    else
    {
        Log::warning("IOStatus", Format("Digital {1} not idle. Current: {2}, Idle: {3}",
                                        Str(name),
                                        Bool(value),
                                        Bool(ref_getter.get_digital_idle(name))));
    }

    return is_idle;
}
