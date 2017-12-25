//
// Created by permal on 12/21/17.
//

#include <sstream>
#include "Config.h"
#include <smooth/core/json/Value.h>
#include <smooth/core/logging/log.h>


using namespace smooth::core::json;
using namespace smooth::core::logging;

std::string Config::get_custom_name(const std::string& short_name)
{
    return io_names[short_name];
}

bool Config::parse(const std::string& data)
{
    bool res = false;

    Log::info("Config", Format("Data prepared, parsing..."));

    auto root = cJSON_Parse(data.c_str());

    if (root)
    {
        const int analog_input_count = 8;
        const int digital_input_count = 8;
        const int digital_output_count = 8;

        Value v(root);

        Log::info("Config", Format("Reading analog input details"));

        for (auto i = 0; i < analog_input_count; ++i)
        {
            std::stringstream ss;
            ss << "a" << i;
            auto input = v["io"]["analog"]["input"][ss.str()];
            io_names[ss.str()] = input["name"].get_string(ss.str());
            Log::info("Analog", Format("Will use name '{1}' for analog input {2}", Str(get_custom_name(ss.str())),
                                       Str(ss.str())));

            auto idle = input["idle"];
            auto value = idle["value"].get_int(0);
            auto variance = idle["variance"].get_int(0);

            analog_idle[ss.str()] = AnalogRef{value, variance};
            Log::info("Analog", Format("{1} ref value: {2}, variance: {3}",
                                       Str(get_custom_name(ss.str())),
                                       Int32(value),
                                       Int32(variance)));
        }

        Log::info("Config", Format("Reading digital input details"));

        for (auto i = 0; i < digital_input_count; ++i)
        {
            std::stringstream ss;
            ss << "i" << i;
            auto input = v["io"]["digital"]["input"][ss.str()];
            io_names[ss.str()] = input["name"].get_string(ss.str());
            Log::info("Digital input", Format("Will use name '{1}' for digital input {2}",
                                              Str(get_custom_name(ss.str())),
                                              Str(ss.str())));

            auto idle = input["idle"]["value"].get_bool(false);

            digital_idle[ss.str()] = idle;
            Log::info("Digital", Format("{1} idle value: {2}",
                                        Str(get_custom_name(ss.str())),
                                        Bool(digital_idle[ss.str()])));
        }

        Log::info("Config", Format("Reading digital output details"));

        for (auto i = 0; i < digital_output_count; ++i)
        {
            std::stringstream ss;
            ss << "o" << i;
            auto output = v["io"]["digital"]["output"][ss.str()];
            io_names[ss.str()] = output["name"].get_string(ss.str());
            digital_startup[ss.str()] = output["startup_state"].get_bool(false);
            Log::info("Digital out",
                      Format("Will use name '{1}' for digital output {2}", Str(get_custom_name(ss.str())),
                             Str(ss.str())));
            Log::info("Digital", Format("{1} startup value: {2}",
                                        Str(get_custom_name(ss.str())),
                                        Bool(digital_startup[ss.str()])));

        }

        Log::info("Config", Format("Reading zones"));

        auto zone_cfg = v["zones"];
        std::vector<std::string> zones_names{};
        zone_cfg.get_member_names(zones_names);
        for (const auto& zone_name : zones_names)
        {
            Log::info("Config", Format("Zone: {1}", Str(zone_name)));
            auto& zone_members = zones[zone_name];

            auto zone_contents = zone_cfg[zone_name];
            for (int i = 0; i < zone_contents.get_array_size(); ++i)
            {
                auto member = zone_contents[i];
                std::string value = static_cast<const char*>(member);
                if(!value.empty())
                {
                    Log::info("Config", Format("Adding {1} to zone {2}", Str(value), Str(zone_name)));
                    zone_members.emplace_back(value);
                }
            }
        }

        cJSON_Delete(root);

        res = true;
    }
    else
    {
        Log::error("Config", Format("Could not parse data as JSON"));
    }

    return res;
}

bool Config::get_digital_idle_state(const std::string& short_name)
{
    return digital_idle[short_name];
}

Config::AnalogRef& Config::get_analog_idle(const std::string& short_name)
{
    return analog_idle[short_name];
}
