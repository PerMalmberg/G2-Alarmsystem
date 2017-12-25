#pragma once

#include <string>
#include <unordered_map>
#include <vector>

class Config
{
    public:
        struct AnalogRef
        {
            AnalogRef() = default;

            AnalogRef(int v, int var) : value(v), variance(var)
            {
            }

            int value = 0;
            int variance = 0;
        };

        bool parse(const std::string& data);
        std::string get_custom_name(const std::string& short_name);
        bool get_digital_idle_state(const std::string& short_name);
        AnalogRef& get_analog_idle(const std::string& short_name);

        const std::unordered_map<std::string, std::vector<std::string>> get_zones() const
        {
            return zones;
        }

    private:
        std::unordered_map<std::string, std::string> io_names{};
        std::unordered_map<std::string, AnalogRef> analog_idle{};
        std::unordered_map<std::string, bool> digital_idle{};
        std::unordered_map<std::string, bool> digital_startup{};
        std::unordered_map<std::string, std::vector<std::string>> zones{};
};
