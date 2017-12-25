#pragma once

#include <string>
#include <unordered_map>

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

    private:
        std::unordered_map<std::string, std::string> io_names{};
        std::unordered_map<std::string, AnalogRef> analog_ref{};
        std::unordered_map<std::string, bool> digital_idle{};
        std::unordered_map<std::string, bool> digital_startup{};
};
