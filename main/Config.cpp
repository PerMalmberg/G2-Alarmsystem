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
using namespace std::chrono;

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
        curr["idle"]["value"].set(digital_idle[name]);
        curr["entry_delay"] = static_cast<int>(digital_entry_delay[name].count());
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
        auto idle = curr["idle"];
        const auto& idle_val = analog_ref[name];
        idle["value"] = idle_val.value;
        idle["variance"] = idle_val.variance;
        curr["entry_delay"] = static_cast<int>(analog_entry_delay[name].count());
    }

    auto z = v["zones"];
    for (auto& zone : zones)
    {
        const auto& name = zone.first;
        const auto& zone_data = zone.second;

        auto curr = z[name];
        auto codes = curr["codes"];
        int i = 0;
        for (auto& code : zone_data.codes)
        {
            codes[i++] = code;
        }

        i = 0;
        auto inputs = curr["inputs"];
        for (auto& input : zone_data.inputs)
        {
            inputs[i++] = input;
        }
    }

    v["exit_delay"] = static_cast<int>(exit_delay.count());
    v["tripped_max_time"] = static_cast<int>(tripped_max_time.count());

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

            analog_ref[name] = AnalogRef{value, variance};

            analog_entry_delay[name] = seconds(input["entry_delay"].get_int(0));

            Log::info("Analog", Format("{1} ref value: {2}, variance: {3}, entry delay: {4}",
                                       Str(get_custom_name(name)),
                                       Int32(value),
                                       Int32(variance),
                                       Int64(analog_entry_delay[name].count())));
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

            digital_idle[name] = idle;

            digital_entry_delay[name] = seconds(input["entry_delay"].get_int(0));

            Log::info("Digital", Format("{1} idle value: {2}, delay: entry delay {3}",
                                        Str(get_custom_name(name)),
                                        Bool(digital_idle[name]),
                                        Int64(digital_entry_delay[name].count())));
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

        auto zone_data = v["zones"];
        std::vector<std::string> zones_names{};
        zone_data.get_member_names(zones_names);
        for (const auto& zone_name : zones_names)
        {
            Log::info("Config", Format("Zone: {1}", Str(zone_name)));
            auto& zone_target = zones[zone_name];
            auto zone_src = zone_data[zone_name];

            auto codes_in_config = zone_src["codes"];

            int code_count = codes_in_config.get_array_size();
            for (int i = 0; i < code_count; ++i)
            {
                auto code = codes_in_config[i].get_string("");
                if (!code.empty())
                {
                    Log::info("Config", Format("Zone {1}, code: {2}", Str(zone_name), Str(code)));
                    zone_target.codes.emplace(code);
                }
            }

            auto inputs_in_config = zone_src["inputs"];
            int input_count = inputs_in_config.get_array_size();
            for (int i = 0; i < input_count; ++i)
            {
                auto input = inputs_in_config[i].get_string("");
                if (!input.empty())
                {
                    Log::info("Config", Format("Zone {1}, input: {2}", Str(zone_name), Str(input)));
                    zone_target.inputs.emplace(input);
                }
            }
        }

        exit_delay = seconds(v["exit_delay"].get_int(0));
        Log::info("Config", Format("Exit delay: {1}", Int64(exit_delay.count())));
        tripped_max_time = seconds(v["tripped_max_time"].get_int(0));

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
    bool res = false;

    auto it = zones.find(alarm_state.get_current_zone());
    if (it != zones.end())
    {
        auto& pair = *it;
        res = pair.second.inputs.find(short_name) != pair.second.inputs.end();
    }

    return res;
}

void Config::set_analog_ref(const std::string& short_name, uint32_t ref_value)
{
    Log::info("Config", Format("Setting analog reference: {1} to {2}", Str(short_name), UInt32(ref_value)));
    auto& ref = analog_ref[short_name];
    ref.value = ref_value;
}

bool Config::has_zone(const std::string& zone_name) const
{
    return !zone_name.empty() && zones.find(zone_name) != zones.end();
}

std::chrono::seconds Config::get_entry_delay(const std::string& input)
{
    seconds delay{0};
    auto analog_it = analog_entry_delay.find(input);
    if (analog_it == analog_entry_delay.end())
    {
        auto digital_it = digital_entry_delay.find(input);
        if (digital_it != digital_entry_delay.end())
        {
            delay = (*digital_it).second;
        }
    }
    else
    {
        delay = (*analog_it).second;
    }

    return delay;
}


