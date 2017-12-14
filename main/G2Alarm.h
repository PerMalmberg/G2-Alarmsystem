#pragma once

#include <memory>
#include <smooth/core/Application.h>
#include <smooth/core/network/Wifi.h>
#include <smooth/core/io/Output.h>
#include <smooth/core/io/Input.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/application/io/MCP23017.h>
#include <smooth/application/io/ADS1115.h>
#include <smooth/application/rgb_led/RGBLed.h>
#include <smooth/application/network/mqtt/MqttClient.h>
#include "AnalogCycler.h"
#include "Wiegand.h"
#include "I2CTask.h"
#include "AnalogValue.h"
#include "DigitalValue.h"
#include "IOStatus.h"
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>

class G2Alarm
        : public smooth::core::Application,
          public IWiegandSignal,
          public smooth::core::ipc::IEventListener<smooth::application::network::mqtt::MQTTData>
{
    public:
        G2Alarm();
        void tick() override;
        void init() override;

        void event(const smooth::application::network::mqtt::MQTTData& event) override;

        void number(uint8_t number) override;
        void id(uint32_t id, uint8_t byte_count) override;

    private:
        smooth::core::io::Output level_shifter_enable;
        smooth::application::rgb_led::RGBLed rgb;
        Wiegand control_panel;
        IOStatus io_status{};
        smooth::core::ipc::SubscribingTaskEventQueue<AnalogValue> analog_data;
        smooth::core::ipc::SubscribingTaskEventQueue<DigitalValue> digital_data;
        smooth::core::ipc::TaskEventQueue<smooth::application::network::mqtt::MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient mqtt;
        std::unique_ptr<I2CTask> i2c{};
};

