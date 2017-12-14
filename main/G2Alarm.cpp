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

using namespace std::chrono;
using namespace smooth::core;
using namespace smooth::core::io;
using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace smooth::application::io;
using namespace smooth::core::network;
using namespace smooth::application::network::mqtt;

G2Alarm::G2Alarm()
        : Application(APPLICATION_BASE_PRIO, milliseconds(50)),
          level_shifter_enable(GPIO_NUM_5, true, false, true),
          rgb(RMT_CHANNEL_0, GPIO_NUM_2, 5, smooth::application::rgb_led::WS2812B()),
          control_panel(*this, *this),
          analog_data("AnalogData", 25, *this, *this),
          digital_data("DigitalData", 25, *this, *this),
          mqtt_data("MQTTData", 10, *this, *this),
          mqtt("Alarm", seconds(10), 4096, 5, mqtt_data)
{
}

void G2Alarm::init()
{
    smooth::core::Application::init();

    level_shifter_enable.set();

    mqtt.connect_to(std::make_shared<IPv4>("192.168.10.245", 1883), true);

    i2c = make_unique<I2CTask>();
    i2c->start();
}

void G2Alarm::tick()
{

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

}

void G2Alarm::event(const AnalogValue& event)
{
    Log::info("Analog", Format("Input: {1}, Value {2}", Int32(event.get_input()), Int32(event.get_value())));
}

void G2Alarm::event(const DigitalValue& event)
{
    Log::info("Digital", Format("Input: {1}, Value {2}", UInt32(event.get_input()), Bool(event.get_value())));
}
