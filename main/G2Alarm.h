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
#include <smooth/application/network/mqtt/MqttClient.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/filesystem/Filesystem.h>
#include <states/AlarmBaseState.h>
#include "AnalogCycler.h"
#include "Wiegand.h"
#include "I2CTask.h"
#include "AnalogValue.h"
#include "DigitalValue.h"
#include "IOStatus.h"
#include "CommandDispatcher.h"
#include "Config.h"
#include "states/AlarmFSM.h"
#include "ArmByNumber.h"
#include "IAlarmState.h"
#include "status_indication/StatusIndicator.h"

class G2Alarm
        : public smooth::core::Application,
          public IWiegandSignal,
          public smooth::core::ipc::IEventListener<smooth::application::network::mqtt::MQTTData>,
          public smooth::core::ipc::IEventListener<AnalogValue>,
          public smooth::core::ipc::IEventListener<DigitalValue>,
          public IAlarmState
{
    public:
        G2Alarm();
        void tick() override;
        void init() override;

        void event(const smooth::application::network::mqtt::MQTTData& event) override;

        void event(const AnalogValue& event) override;
        void event(const DigitalValue& event) override;

        void wiegand_number(uint8_t number) override;
        void wiegand_id(uint32_t id, uint8_t byte_count) override;

        bool is_armed() const override;
        bool is_arming() const override;
        void arm(const std::string& zone) override;
        void disarm() override;

        std::string get_current_zone() const override
        {
            return current_zone;
        }

    private:
        smooth::core::io::Output level_shifter_enable;

        Wiegand control_panel;
        Config cfg;
        ArmByNumber arm_by_number;
        IOStatus io_status;
        smooth::core::ipc::SubscribingTaskEventQueue<AnalogValue> analog_data;
        smooth::core::ipc::SubscribingTaskEventQueue<DigitalValue> digital_data;
        smooth::core::ipc::TaskEventQueue<smooth::application::network::mqtt::MQTTData> mqtt_data;
        smooth::application::network::mqtt::MqttClient mqtt;
        std::unique_ptr<I2CTask> i2c{};
        CommandDispatcher command_dispatcher{};
        smooth::core::timer::ElapsedTime uptime{};
        smooth::core::timer::ElapsedTime mqtt_send_period{};
        AlarmFSM<AlarmBaseState> fsm;
        std::string current_zone{};
        StatusIndicator status_indicator{};


        void read_configuration();

        std::string get_name() const
        {
            return "g2";
        }
};

