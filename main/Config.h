#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include "AnalogRef.h"
#include "IReferenceValueGetter.h"

class Config : public IReferenceValueGetter
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


        const std::unordered_map<std::string, std::vector<std::string>> get_zones() const
        {
            return zones;
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
    private:
        std::unordered_map<std::string, std::string> io_names{};
        std::unordered_map<std::string, bool> io_enabled{};
        std::unordered_map<std::string, AnalogRef> analog_ref{};
        std::unordered_map<std::string, bool> digital_idle{};
        std::unordered_map<std::string, bool> digital_startup{};
        std::unordered_map<std::string, std::vector<std::string>> zones{};
        std::vector<std::string> codes{};

        const std::vector<std::string> digital_input_names{"i0", "i1", "i2", "i3", "i4", "i4", "i6", "i7"};
        const std::vector<std::string> digital_output_names{"o0", "o1", "o2", "o3", "o4", "o4", "o6", "o7"};
        const std::vector<std::string> analog_input_names{"a10", "a11", "a12", "a13", "a20", "a21", "a22", "a23"};

};
