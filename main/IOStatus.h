#pragma once

#include <unordered_map>
#include <string>
#include "AnalogValue.h"
#include "DigitalValue.h"
#include "IReferenceValueGetter.h"

#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>

class IOStatus
        : public smooth::core::ipc::IEventListener<AnalogValue>,
          public smooth::core::ipc::IEventListener<DigitalValue>
{
    public:
        explicit IOStatus(IReferenceValueGetter& ref_getter)
                : ref_getter(ref_getter)
        {
        }

        void event(const AnalogValue& event) override;
        void event(const DigitalValue& event) override;

        const std::unordered_map<std::string, bool>& get_digital_values() const
        {
            return digital_values;
        };

        const std::unordered_map<std::string, uint32_t>& get_analog_values() const
        {
            return analog_values;
        };

    private:
        IReferenceValueGetter& ref_getter;
        std::unordered_map<std::string, bool> digital_values{};
        std::unordered_map<std::string, uint32_t> analog_values{};


};