//
// Created by permal on 12/21/17.
//

#include <sstream>
#include "Config.h"
#include <smooth/core/json/Value.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/filesystem/File.h>


using namespace smooth::core::json;
using namespace smooth::core::logging;
using namespace smooth::core::filesystem;

std::string Config::get_custom_name(const std::string& short_name)
{
    return io_names[short_name];
}

bool Config::write(const std::string& file)
{
    Value v{};

    auto dig_input = v["io"]["digital"]["input"];

    for (const auto& name : digital_input_names)
    {
        auto curr = dig_input[name];
        curr["name"] = io_names[name];
        curr["enabled"].set(io_enabled[name]);
        curr["idle"]["value"].set(digital_idle[name]);
    }

    auto dig_output = v["io"]["digital"]["output"];
    for (const auto& name : digital_output_names)
    {
        auto curr = dig_output[name];
        curr["name"] = io_names[name];
        curr["startup_state"].set(digital_startup[name]);
    }

    auto analog_input = v["io"]["analog"]["input"];
    for (const auto& name : analog_input_names)
    {
        auto curr = analog_input[name];
        curr["name"] = io_names[name];
        curr["enabled"].set(io_enabled[name]);
        auto idle = curr["idle"];
        const auto& idle_val = analog_ref[name];
        idle["value"] = idle_val.value;
        idle["variance"] = idle_val.variance;
    }

    auto z = v["zones"];
    for (auto& zone : zones)
    {
        const auto& name = zone.first;
        const auto& content = zone.second;
        auto curr = z[name];

        int i = 0;
        for (const auto& input : content)
        {
            curr[i++] = input;
        }
    }

    auto codes = v["codes"];
    int code_count = 0;
    for(auto& code : this->codes)
    {
        codes[code_count++] = code;
    }

    File f(file);
    return f.write(v.to_string());
}

bool Config::read(const std::string& file)
{
    std::vector<uint8_t> data;

    bool res = false;

    File f(file);
    if (f.read(data))
    {
        data.emplace_back(0); // Ensure ending null terminator.
        res = parse(reinterpret_cast<const char*>(data.data()));
    }

    return res;
}

bool Config::parse(const char* data)
{
    bool res = false;

    auto root = cJSON_Parse(data);

    if (root)
    {
        Value v(root, true);

        Log::info("Config", Format("Reading analog input details"));

        for (const auto& name : analog_input_names)
        {
            auto input = v["io"]["analog"]["input"][name];
            io_names[name] = input["name"].get_string(name);
            Log::info("Analog", Format("Will use name '{1}' for analog input {2}", Str(get_custom_name(name)),
                                       Str(name)));

            auto idle = input["idle"];
            auto value = idle["value"].get_int(0);
            auto variance = idle["variance"].get_int(0);
            auto enabled = input["enabled"].get_bool(false);

            io_enabled[name] = enabled;
            analog_ref[name] = AnalogRef{value, variance};
            Log::info("Analog", Format("{1} ref value: {2}, variance: {3}, enabled: {4}",
                                       Str(get_custom_name(name)),
                                       Int32(value),
                                       Int32(variance),
                                       Bool(enabled)));
        }

        Log::info("Config", Format("Reading digital input details"));

        for (const auto& name : digital_input_names)
        {
            auto input = v["io"]["digital"]["input"][name];
            io_names[name] = input["name"].get_string(name);
            Log::info("Digital input", Format("Will use name '{1}' for digital input {2}",
                                              Str(get_custom_name(name)),
                                              Str(name)));

            auto idle = input["idle"]["value"].get_bool(false);
            auto enabled = input["enabled"].get_bool(false);

            digital_idle[name] = idle;
            io_enabled[name] = enabled;

            Log::info("Digital", Format("{1} idle value: {2}, enabled: {3}",
                                        Str(get_custom_name(name)),
                                        Bool(digital_idle[name]),
                                        Bool(enabled)));
        }

        Log::info("Config", Format("Reading digital output details"));

        for (const auto& name : digital_output_names)
        {
            auto output = v["io"]["digital"]["output"][name];
            io_names[name] = output["name"].get_string(name);
            digital_startup[name] = output["startup_state"].get_bool(false);
            Log::info("Digital out",
                      Format("Will use name '{1}' for digital output {2}", Str(get_custom_name(name)),
                             Str(name)));
            Log::info("Digital", Format("{1} startup value: {2}",
                                        Str(get_custom_name(name)),
                                        Bool(digital_startup[name])));

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
                if (!value.empty())
                {
                    Log::info("Config", Format("Adding {1} to zone {2}", Str(value), Str(zone_name)));
                    zone_members.emplace_back(value);
                }
            }
        }

        Log::info("Config", Format("Reading codes"));
        auto codes = v["codes"];
        for(int i = 0; i < codes.get_array_size(); ++i)
        {
            std::string c = codes[i].get_string("");
            if(!c.empty())
            {
                Log::info("Config", Format("Code: {1}", Str(c)));
                this->codes.push_back(c);
            }
        }

        res = true;
    }
    else
    {
        Log::error("Config", Format("Could not parse data as JSON"));
    }

    return res;
}

const AnalogRef& Config::get_analog_reference(const std::string& short_name)
{
    return analog_ref[short_name];
}

bool Config::get_digital_idle(const std::string& short_name)
{
    return digital_idle[short_name];
}

bool Config::get_digital_startup_state(const std::string& short_name)
{
    return digital_startup[short_name];
}

bool Config::is_input_enabled(const std::string& short_name)
{
    return io_enabled[short_name];
}

void Config::set_analog_ref(const std::string& short_name, uint32_t ref_value)
{
    Log::info("Config", Format("Setting analog reference: {1} to {2}", Str(short_name), UInt32(ref_value)));
    auto& ref = analog_ref[short_name];
    ref.value = ref_value;
}


