#pragma once

#include <unordered_map>
#include <string>
#include "AnalogValue.h"
#include "DigitalValue.h"

#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/ipc/Publisher.h>

class IOStatus
        : public smooth::core::ipc::IEventListener<AnalogValue>,
          public smooth::core::ipc::IEventListener<DigitalValue>
{
    public:
        IOStatus() = default;

        void event(const AnalogValue& event) override;
        void event(const DigitalValue& event) override;

        void arm();

    private:
        std::unordered_map<int, uint32_t> analog_status{};
        std::unordered_map<int, uint32_t> analog_status_reference{};
        std::unordered_map<uint32_t, bool> digital_status{};
        std::unordered_map<uint32_t, bool> digital_status_reference{};

        template<typename T, typename U>
        bool compare_equal(const std::unordered_map<T, U>& previous,
                           const std::unordered_map<T, U>& current) const
        {
            bool res = true;
            for (auto& prev_pair : previous)
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
        bool compare_diff(const std::unordered_map<T, U>& previous,
                          const std::unordered_map<T, U>& current,
                          int64_t max_diff) const
        {
            bool res = true;
            for (auto& prev_pair : previous)
            {
                auto in_current = current.find(prev_pair.first);
                if (in_current != current.end())
                {
                    int64_t prev = prev_pair.second;
                    int64_t curr = (*in_current).second;

                    int64_t diff = std::max(prev, curr) - std::min(prev, curr);
                    res &= diff < max_diff;
                }
            }

            return res;
        }
};