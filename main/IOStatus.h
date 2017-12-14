#pragma once

#include <unordered_map>
#include "AnalogValue.h"
#include "DigitalValue.h"

#include <smooth/core/ipc/SubscribingTaskEventQueue.h>

class IOStatus
        :
                public smooth::core::ipc::IEventListener<AnalogValue>,
                public smooth::core::ipc::IEventListener<DigitalValue>
{
    public:
        IOStatus();

        void event(const AnalogValue& event) override;
        void event(const DigitalValue& event) override;

    private:
        std::unordered_map<int, uint32_t> analog_status{};
        std::unordered_map<int, uint32_t> analog_status_prev{};
        std::unordered_map<uint32_t, bool> digital_status{};
        std::unordered_map<uint32_t, bool> digital_status_prev{};

        template<typename T, typename U>
        bool compare_equal(const std::unordered_map<T, U>& previous, const std::unordered_map<T, U>& current) const
        {
            bool res = true;
            for (auto prev_pair : previous)
            {
                auto in_current = current.find(prev_pair.first);
                if (in_current != current.end())
                {
                    res &= prev_pair.second == (*in_current).second;
                }
            }

            return res;
        }

        template<typename T, typename U>
        bool compare_diff(const std::unordered_map<T, U>& previous, const std::unordered_map<T, U>& current,
                          uint32_t max_diff) const
        {
            bool res = true;
            for (auto prev_pair : previous)
            {
                auto in_current = current.find(prev_pair.first);
                if (in_current != current.end())
                {
                    auto a = prev_pair.second;
                    auto b = (*in_current).second;
                    res &= std::max(a, b) - std::min(a, b) < max_diff;
                }
            }

            return res;
        }
};
