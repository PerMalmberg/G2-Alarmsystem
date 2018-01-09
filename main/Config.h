#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <set>
#include "AnalogRef.h"
#include "IReferenceValueGetter.h"
#include "ZoneData.h"

class Config
        : public IReferenceValueGetter
{
    public:

        bool parse(const char* data);
        bool write(const std::string& file);
        bool read(const std::string& file);

        std::string get_custom_name(const std::string& short_name);

        const AnalogRef& get_analog_reference(const std::string& short_name) override;
        bool get_digital_idle(const std::string& short_name) override;
        bool get_digital_startup_state(const std::string& short_name) override;
        bool is_input_enabled(const std::string& short_name) override;


        bool has_zone(const std::string& zone_name) const;

        bool has_zone_with_code(const std::string& code)
        {
            bool res = false;

            for(auto it = zones.begin(); !res && it != zones.end(); it++)
            {
                auto& curr = *it;
                res = curr.second.has_code(code);
            }

            return res;
        }

        const std::string get_zone_for_code(const std::string& code) const
        {
            std::string res{};

            for(auto it = zones.begin(); res.empty() && it != zones.end(); it++)
            {
                auto& curr = *it;
                if(curr.second.has_code(code))
                {
                    res = curr.first;
                }
            }

            return res;
        }

        const std::vector<std::string> get_digital_input_names() override
        {
            return digital_input_names;
        }

        const std::vector<std::string> get_analog_input_names() override
        {
            return analog_input_names;
        }

        void set_analog_ref(const std::string& short_name, uint32_t ref_value);

        void set_current_zone(const std::string& name)
        {
            current_zone = name;
        }

    private:
        std::unordered_map<std::string, std::string> io_names{};
        std::unordered_map<std::string, AnalogRef> analog_ref{};
        std::unordered_map<std::string, bool> digital_idle{};
        std::unordered_map<std::string, bool> digital_startup{};
        std::unordered_map<std::string, ZoneData> zones{};

        const std::vector<std::string> digital_input_names{"i0", "i1", "i2", "i3", "i4", "i4", "i6", "i7"};
        const std::vector<std::string> digital_output_names{"o0", "o1", "o2", "o3", "o4", "o4", "o6", "o7"};
        const std::vector<std::string> analog_input_names{"a10", "a11", "a12", "a13", "a20", "a21", "a22", "a23"};

        std::string current_zone{};
};
