#pragma once

#include <unordered_map>
#include <string>
#include "AnalogValue.h"
#include "DigitalValue.h"
#include "IReferenceValueGetter.h"

#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>

class IOStatus
{
    public:
        explicit IOStatus(IReferenceValueGetter& ref_getter)
                : ref_getter(ref_getter)
        {
        }

        bool event(const AnalogValue& event);
        bool event(const DigitalValue& event);

        const std::unordered_map<std::string, bool>& get_digital_values() const
        {
            return digital_values;
        }

        const std::unordered_map<std::string, uint32_t>& get_analog_values() const
        {
            return analog_values;
        }

    private:
        IReferenceValueGetter& ref_getter;
        std::unordered_map<std::string, bool> digital_values{};
        std::unordered_map<std::string, uint32_t> analog_values{};


};