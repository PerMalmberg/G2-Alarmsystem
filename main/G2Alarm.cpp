//
// Created by permal on 11/24/17.
//

#include "G2Alarm.h"
#include <smooth/core/task_priorities.h>
#include <smooth/core/filesystem/File.h>
#include <states/Idle.h>
#include <smooth/core/json/Value.h>

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
        : Application(APPLICATION_BASE_PRIO, milliseconds(100)),
          level_shifter_enable(GPIO_NUM_5, true, false, true),
          control_panel(*this, *this),
          cfg(*this),
          arm_by_number(cfg, *this, 10, 10, 11, milliseconds(1500)),
          io_status(cfg),
          analog_data("AnalogData", 25, *this, *this),
          digital_data("DigitalData", 25, *this, *this),
          mqtt_data("MQTTData", 10, *this, *this),
          mqtt("Alarm", seconds(10), 4096, 5, mqtt_data),
          general_message("General message", 10, *this, *this),
          fsm(io_status, cfg, *this)
{
}

void G2Alarm::init()
{
    smooth::core::Application::init();

    fsm.set_state(new(fsm) Idle(fsm));

    uptime.start();
    mqtt_send_period.start();
    read_configuration();

    mqtt.subscribe(get_name() + "/cmd/#", QoS::EXACTLY_ONCE);
    command_dispatcher.add_command(get_name() + "/cmd/write_config", [this](const std::string& o)
    {
        try
        {
            // Parse the config before writing it to disk
            Config tmp(*this);
            if (tmp.parse(o.data()))
            {
                // Store new config
                if (tmp.write(CONFIG_FILE))
                {
                    // Now read the new config.
                    if (cfg.read(CONFIG_FILE))
                    {
                        mqtt.publish(get_name() + "/response/write_config/result", "1", QoS::AT_LEAST_ONCE, false);
                    }
                    else
                    {
                        mqtt.publish(get_name() + "/response/write_config/result",
                                     "0",
                                     QoS::AT_LEAST_ONCE,
                                     false);
                        mqtt.publish(get_name() + "/response/write_config/result_message",
                                     "Could not read config",
                                     QoS::AT_LEAST_ONCE,
                                     false);
                    }
                }
                else
                {
                    mqtt.publish(get_name() + "/response/write_config/result", "0", QoS::AT_LEAST_ONCE, false);
                    mqtt.publish(get_name() + "/response/write_config/result_message",
                                 "Could not write config",
                                 QoS::AT_LEAST_ONCE,
                                 false);
                }
            }
            else
            {
                mqtt.publish(get_name() + "/response/write_config/result",
                             "0",
                             QoS::AT_LEAST_ONCE,
                             false);
                mqtt.publish(get_name() + "/response/write_config/result_message",
                             "Could not parse config",
                             QoS::AT_LEAST_ONCE,
                             false);
            }
        }
        catch (std::exception& ex)
        {
            mqtt.publish(get_name() + "/response/write_config/result", "0", QoS::AT_LEAST_ONCE, false);
            mqtt.publish(get_name() + "/response/write_config/result_message", ex.what(), QoS::AT_LEAST_ONCE, false);
        }
    });

    command_dispatcher.add_command(get_name() + "/cmd/read_config", [this](const std::string& o)
    {
        try
        {
            std::vector<uint8_t> data;
            File f(CONFIG_FILE);

            if (f.read(data))
            {
                mqtt.publish(get_name() + "/response/read_config/data",
                             data.data(),
                             static_cast<int>(data.size()),
                             QoS::AT_LEAST_ONCE, false);

                mqtt.publish(get_name() + "/response/read_config/result", "1", QoS::AT_LEAST_ONCE, false);
            }
            else
            {
                mqtt.publish(get_name() + "/response/read_config/result", "0", QoS::AT_LEAST_ONCE, false);
            }

        }
        catch (std::exception& ex)
        {
            mqtt.publish(get_name() + "/response/read_config/result", "0", QoS::AT_LEAST_ONCE, false);
            mqtt.publish(get_name() + "/response/read_config/result_message", ex.what(), QoS::AT_LEAST_ONCE, false);
        }
    });

    command_dispatcher.add_command(get_name() + "/cmd/store_current_config", [this](const std::string& o)
    {
        mqtt.publish(get_name() + "/response/store_current_config/result",
                     cfg.write(CONFIG_FILE) ? "1" : "0",
                     QoS::AT_LEAST_ONCE,
                     false);
    });

    command_dispatcher.add_command(get_name() + "/cmd/set_references", [this](const std::string& o)
    {
        for (const auto& pair : io_status.get_analog_values())
        {
            cfg.set_analog_ref(pair.first, pair.second);
        }
        mqtt.publish(get_name() + "/response/set_references/result", "1", QoS::AT_LEAST_ONCE, false);
    });

    command_dispatcher.add_command(get_name() + "/cmd/arm", [this](const std::string& o)
    {
        if (cfg.has_zone(o))
        {
            set_current_zone(o);
            fsm.arm(o);
            Log::info("Alarm", Format("Arming zone: {1}", Str(o)));
        }
        else
        {
            Log::error("Alarm", Format("No such zone: {1}", Str(o)));
        }
    });

    command_dispatcher.add_command(get_name() + "/cmd/arm_code", [this](const std::string& o)
    {
        if (cfg.has_zone_with_code(o))
        {
            auto& zone = cfg.get_zone_for_code(o);
            set_current_zone(zone);
            fsm.arm(zone);
            Log::info("Alarm", Format("Arming zone: {1}", Str(zone)));
        }
        else
        {
            Log::error("Alarm", Format("No zone for code: {1}", Str(o)));
        }
    });


    command_dispatcher.add_command(get_name() + "/cmd/disarm", [this](const std::string& o)
    {
        disarm();
    });

    level_shifter_enable.set();

    mqtt.connect_to(std::make_shared<IPv4>("192.168.10.245", 1883), true);


    i2c = make_unique<I2CTask>();
    i2c->start();


    command_dispatcher.add_command(get_name() + "/cmd/rgb", [this](const std::string& o)
    {
        json::Value v(o);

        uint8_t i = static_cast<uint8_t>(v["i"].get_int(0));
        uint8_t r = static_cast<uint8_t>(v["r"].get_int(0));
        uint8_t g = static_cast<uint8_t>(v["g"].get_int(0));
        uint8_t b = static_cast<uint8_t>(v["b"].get_int(0));

        Log::warning("LED", Format("{1} {2} {3} {4}", UInt32(i), UInt32(r), UInt32(g), UInt32(b)));

        fsm.set_pixel(i, r, g, b);
        fsm.apply_rgb();
    });
}

void G2Alarm::tick()
{
    auto up = uptime.get_running_time();
    std::chrono::seconds s = std::chrono::duration_cast<seconds>(up);

    std::string msg = Format("{1}", Int64(s.count())).get();

    if (mqtt_send_period.get_running_time() > seconds(1))
    {
        mqtt.publish(get_name() + "/status/uptime",
                     msg,
                     QoS::AT_MOST_ONCE,
                     false);
        mqtt_send_period.reset();

        mqtt.publish(get_name() + "/status/state",
                     fsm.get_state_name(),
                     QoS::AT_MOST_ONCE,
                     false);

        mqtt.publish(get_name() + "/status/zone",
                     get_current_zone(),
                     QoS::AT_MOST_ONCE,
                     false);
    }

    fsm.tick();
    arm_by_number.tick();
}


void G2Alarm::wiegand_number(uint8_t number)
{
    arm_by_number.number(number);
    mqtt.publish("Wiegand/key", std::to_string(number), QoS::AT_MOST_ONCE, false);
}

void G2Alarm::wiegand_id(uint32_t id, uint8_t /*byte_count*/)
{
    auto s = std::to_string(id);
    while (!s.empty())
    {
        std::string part{};
        part += *s.begin();
        s.erase(s.begin());

        arm_by_number.number(std::atoi(part.c_str()));
    }
    arm_by_number.commit();

    mqtt.publish("Wiegand/card", std::to_string(id), QoS::AT_MOST_ONCE, false);
}

void G2Alarm::event(const smooth::application::network::mqtt::MQTTData& event)
{
    command_dispatcher.process(event.first, MqttClient::get_payload(event));
}

void G2Alarm::event(const std::pair<std::string, int64_t>& event)
{
    mqtt.publish(event.first, std::to_string(event.second), QoS::AT_MOST_ONCE, false);
}

void G2Alarm::read_configuration()
{
    Log::info("Config", Format("Reading configuration file..."));
    if (!cfg.read(CONFIG_FILE))
    {
        Log::error("Config", Format("Could not parse configuration"));
    }
}

void G2Alarm::event(const AnalogValue& event)
{
    if (io_status.event(event))
    {
        std::string topic = get_name();
        topic += "/io_status/analog/";
        topic += event.get_name();
        auto& ref = cfg.get_analog_reference(event.get_name());
        json::Value v{};
        v["current"] = static_cast<int>(event.get_value());
        v["idle_value"] = ref.value;
        v["variance"] = ref.variance;
        v["min"] = ref.value - ref.variance;
        v["max"] = ref.value + ref.variance;
        v["is_idle"].set(io_status.is_analog_inside_limit(event.get_name(), event.get_value()));
        mqtt.publish(topic, v.to_string(), QoS::AT_MOST_ONCE, false);
    }
}

void G2Alarm::event(const DigitalValue& event)
{
    if (io_status.event(event))
    {
        std::string topic = get_name();
        topic += "/io_status/digital/i";
        topic += event.get_name();

        json::Value v{};
        v["current"] = event.get_value();
        v["idle_value"] = cfg.get_digital_idle(event.get_name());
        v["is_idle"].set(io_status.is_digital_idle(event.get_name(), event.get_value()));
        mqtt.publish(topic, v.to_string(), QoS::AT_MOST_ONCE, false);
    }
}

bool G2Alarm::is_armed() const
{
    return fsm.is_armed();
}

bool G2Alarm::is_arming() const
{
    return fsm.is_arming();
}

void G2Alarm::arm(std::string& zone)
{
    fsm.arm(zone);
}

void G2Alarm::disarm()
{
    fsm.disarm();
}
