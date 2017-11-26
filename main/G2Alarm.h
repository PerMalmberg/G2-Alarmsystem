#pragma once

#include <memory>
#include <smooth/core/Application.h>
#include <smooth/core/io/Output.h>
#include <smooth/core/io/Input.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/io/i2c/Master.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/application/io/MCP23017.h>
#include <smooth/application/io/ADS1115.h>

class G2Alarm
        : public smooth::core::Application,
          public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>
{
    public:
        G2Alarm();
        void tick() override;
        void init() override;

        void event(const smooth::core::io::InterruptInputEvent& ev);

    private:
        smooth::core::io::Output i2c_power;
        smooth::core::io::i2c::Master digital_i2c_master;
        smooth::core::io::i2c::Master analog_i2c_master;
        std::unique_ptr<smooth::application::io::MCP23017> digital_io{};
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 5> input_change_queue;
        smooth::core::io::InterruptInput digital_input_change;
        //std::unique_ptr<smooth::application::io::ADS1115> analog_io_1{};
        //smooth::core::io::InterruptInput analog_change_1;

};

