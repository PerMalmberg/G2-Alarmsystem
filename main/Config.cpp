//
// Created by permal on 12/21/17.
//

#include <sstream>
#include "Config.h"
#include <smooth/core/json/Json.h>
#include <smooth/core/logging/log.h>


using namespace smooth::core::json;
using namespace smooth::core::logging;

bool Config::parse(const std::string& data)
{
    bool res = false;

    Log::info("Config", Format("Data prepared, parsing..."));

    auto root = cJSON_Parse(data.c_str());

    if (root)
    {
        Value v(root);

        Log::info("Config", Format("Data prepared, reading analog input names"));
        auto analog_names = v["io"]["naming"]["analog"];

        for (auto i = 0; i < 8; ++i)
        {
            std::stringstream ss;
            ss << "a" <<  i;
            auto name = analog_names[ss.str()];
            io_names[ss.str()] = name.get_string(ss.str());
            Log::info("Analog", Format("Will use name '{1}' for analog input {2}", Str(io_names[ss.str()]), Str(ss.str())));
        }

        Log::info("Config", Format("Data prepared, reading digital input names"));
        auto digital_name_input = v["io"]["naming"]["digital"]["input"];

        for (auto i = 0; i < 8; ++i)
        {
            std::stringstream ss;
            ss << "i" <<  i;
            auto name = digital_name_input[ss.str()];
            io_names[ss.str()] = name.get_string(ss.str());
            Log::info("Digital input", Format("Will use name '{1}' for digital input {2}", Str(io_names[ss.str()]), Str(ss.str())));
        }



        Log::info("Config", Format("Data prepared, reading digital output names"));
        auto digital_name_output = v["io"]["naming"]["digital"]["output"];

        for (auto i = 0; i < 8; ++i)
        {
            std::stringstream ss;
            ss << "o" <<  i;
            auto name = digital_name_output[ss.str()];
            io_names[ss.str()] = name.get_string(ss.str());
            Log::info("Digital out", Format("Will use name '{1}' for digital output {2}", Str(io_names[ss.str()]), Str(ss.str())));
        }


        cJSON_Delete(root);

        res  = true;
    }
    else
    {
        Log::error("Config", Format("Could not parse data as JSON"));
    }

    return res;
}
