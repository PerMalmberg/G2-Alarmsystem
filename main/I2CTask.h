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
#include "AnalogCycler.h"

class I2CTask
        : public smooth::core::Task,
          public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>
{
    public:
        I2CTask();
        void tick() override;
        void init() override;

        void event(const smooth::core::io::InterruptInputEvent& ev) override;

    private:
        smooth::core::io::Output i2c_power;
        smooth::core::io::i2c::Master digital_i2c_master;
        smooth::core::io::i2c::Master analog_i2c_master;
        std::unique_ptr<smooth::application::io::MCP23017> digital_io{};
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5> input_change_queue;
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5> analog_change_queue_1;
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5> analog_change_queue_2;
        smooth::core::io::InterruptInput digital_input_change;
        smooth::core::io::InterruptInput analog_change_1;
        smooth::core::io::InterruptInput analog_change_2;
        std::unique_ptr<AnalogCycler> cycler_1{};
        std::unique_ptr<AnalogCycler> cycler_2{};

        uint8_t out = 0;

        void update_inputs();
};

