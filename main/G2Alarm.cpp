//
// Created by permal on 11/24/17.
//

#include "G2Alarm.h"
#include <smooth/core/io/i2c/Master.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/util/ByteSet.h>
#include <driver/i2c.h>
#include <chrono>
#include <thread>
#include <smooth/core/util/make_unique.h>
#include "wifi-creds.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/filesystem/File.h>

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::io;
using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace smooth::core::filesystem;
using namespace smooth::application::io;
using namespace smooth::core::network;
using namespace smooth::application::network::mqtt;

const char* CONFIG_FILE = "/config/config.jsn";

G2Alarm::G2Alarm()
        : Application(APPLICATION_BASE_PRIO, milliseconds(1000)),
          level_shifter_enable(GPIO_NUM_5, true, false, true),
          rgb(RMT_CHANNEL_0, GPIO_NUM_2, 5, smooth::application::rgb_led::WS2812B()),
          control_panel(*this, *this),
          io_status(cfg),
          analog_data("AnalogData", 25, *this, io_status),
          digital_data("DigitalData", 25, *this, io_status),
          mqtt_data("MQTTData", 10, *this, *this),
          mqtt("Alarm", seconds(10), 4096, 5, mqtt_data),
          general_message("General message", 10, *this, *this)
{
}

void G2Alarm::init()
{
    smooth::core::Application::init();

    read_configuration();

    mqtt.subscribe("g2/cmd/#", QoS::EXACTLY_ONCE);
    command_dispatcher.add_command("g2/cmd/write_config", [this](const std::string& o)
    {
        try
        {
            // Parse the config before writing it to disk
            Config tmp;
            if (tmp.parse(o.data()))
            {
                if (tmp.write(CONFIG_FILE) && cfg.read(CONFIG_FILE))
                {
                    mqtt.publish("g2/response/write_config/result", "1", QoS::AT_LEAST_ONCE, false);
                }
                else
                {
                    mqtt.publish("g2/response/write_config/result", "0", QoS::AT_LEAST_ONCE, false);
                }
            }
            else
            {
                mqtt.publish("g2/response/write_config/result", "0", QoS::AT_LEAST_ONCE, false);
                mqtt.publish("g2/response/write_config/result_message", "Could not parse config", QoS::AT_LEAST_ONCE,
                             false);
            }
        }
        catch (std::exception& ex)
        {
            mqtt.publish("g2/response/write_config/result", "0", QoS::AT_LEAST_ONCE, false);
            mqtt.publish("g2/response/write_config/result_message", ex.what(), QoS::AT_LEAST_ONCE, false);
        }
    });

    command_dispatcher.add_command("g2/cmd/read_config", [this](const std::string& o)
    {
        try
        {
            std::vector<uint8_t> data;
            File f(CONFIG_FILE);

            if (f.read(data))
            {
                mqtt.publish("g2/response/read_config/data",
                             data.data(),
                             static_cast<int>(data.size()),
                             QoS::AT_LEAST_ONCE, false);

                mqtt.publish("g2/response/read_config/result", "1", QoS::AT_LEAST_ONCE, false);
            }
            else
            {
                mqtt.publish("g2/response/read_config/result", "0", QoS::AT_LEAST_ONCE, false);
            }

        }
        catch (std::exception& ex)
        {
            mqtt.publish("g2/response/read_config/result", "0", QoS::AT_LEAST_ONCE, false);
            mqtt.publish("g2/response/read_config/result_message", ex.what(), QoS::AT_LEAST_ONCE, false);
        }
    });

    command_dispatcher.add_command("g2/cmd/store_reference", [this](const std::string& o)
    {
        try
        {


        }
        catch (std::exception& ex)
        {

        }
    });

    command_dispatcher.add_command("g2/cmd/arm", [this](const std::string& o)
    {
        if (o == "1")
        {
            io_status.arm();
        }
    });


    level_shifter_enable.set();

    mqtt.connect_to(std::make_shared<IPv4>("192.168.10.245", 1883), true);


    i2c = make_unique<I2CTask>();
    i2c->start();
}

void G2Alarm::tick()
{
    static bool toggle = false;
    rgb.set_pixel(0, 0, static_cast<uint8_t>(toggle ? 0 : 5), 0);
    rgb.apply();
    toggle = !toggle;
}


void G2Alarm::number(uint8_t number)
{
    UInt32 s(number);
    mqtt.publish("Wiegand/8", s.str(), QoS::AT_MOST_ONCE, false);

}

void G2Alarm::id(uint32_t id, uint8_t byte_count)
{
    UInt32 s(id);
    mqtt.publish("Wiegand/24", s.str(), QoS::AT_MOST_ONCE, false);
}

void G2Alarm::event(const smooth::application::network::mqtt::MQTTData& event)
{
    command_dispatcher.process(event.first, MqttClient::get_payload(event));
}

void G2Alarm::event(const std::pair<std::string, int64_t>& event)
{
    std::stringstream ss;
    ss << event.second;
    mqtt.publish(event.first, ss.str(), QoS::AT_MOST_ONCE, false);
}

void G2Alarm::read_configuration()
{
    Log::info("Config", Format("Reading configuration file..."));
    if (!cfg.read(CONFIG_FILE))
    {
        Log::error("Config", Format("Could not parse configuration"));
    }
}

