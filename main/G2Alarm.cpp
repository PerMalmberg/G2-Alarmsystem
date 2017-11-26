//
// Created by permal on 11/24/17.
//

#include "G2Alarm.h"
#include <smooth/core/io/i2c/Master.h>
#include <smooth/core/logging/log.h>
#include <smooth/core/util/ByteSet.h>
#include <driver/i2c.h>
#include <chrono>

using namespace std::chrono;
using namespace smooth::core::io;
using namespace smooth::core::logging;
using namespace smooth::application::io;

static const gpio_num_t DIGITAL_CHANGE_PIN = GPIO_NUM_33;
static const gpio_num_t ANALOG_CHANGE_PIN_1 = GPIO_NUM_34;

G2Alarm::G2Alarm()
        : Application(5, seconds(1)),
          i2c_power(GPIO_NUM_27, true, false, true),
          digital_i2c_master(I2C_NUM_0, GPIO_NUM_16, false, GPIO_NUM_17, false, 100000),
          analog_i2c_master(I2C_NUM_1, GPIO_NUM_25, false, GPIO_NUM_26, false, 1000000),
          input_change_queue(*this, *this),
          digital_input_change(input_change_queue, DIGITAL_CHANGE_PIN, false, false, GPIO_INTR_ANYEDGE)//,
          //analog_change_1(input_change_queue, ANALOG_CHANGE_PIN_1, true, false)
{

}

void G2Alarm::init()
{
    i2c_power.clr();
    std::this_thread::sleep_for(milliseconds(5));
    i2c_power.set();

    digital_io = digital_i2c_master.create_device<MCP23017>(0x20);

    if (digital_io->is_present())
    {
        auto conf = digital_io->configure_device(
                false,// mirror_change_interrupt
                false, // interrupt_polarity_active_high
                // Port A are all inputs
                0xFF, // interrupt_on_change_enable_a
                0x00, // interrupt_control_register_a
                0x00, // interrupt_default_val_a
                // Port B are all outputs
                0x00, // interrupt_on_change_enable_b,
                0x00, // interrupt_control_register_b,
                0x00  // interrupt_default_val_b
        );

        Log::info("DigitalIO", Format("Configure device: {1}", Bool(conf)));

        conf = digital_io->configure_ports(
                // Port A are all inputs
                0xFF,
                0x00,
                0xFF,
                // Port B are all outputs
                0x00,
                0x00,
                0x00
        );

        Log::info("DigitalIO", Format("Configure ports: {1}", Bool(conf)));
    }
    else
    {
        Log::error("DigitalIO", Format("Not present"));
    }

//    analog_io_1 = analog_i2c_master.create_device<ADS1115>(0x48);
//
//    if (analog_io_1->is_present())
//    {
//        auto conf = analog_io_1->configure(ADS1115::Multiplexer::Single_AIN1,
//                               ADS1115::Range::FSR_6_144,
//                               ADS1115::OperationalMode::Continuous,
//                               ADS1115::DataRate::SPS_32,
//                               ADS1115::ComparatorMode::Traditional,
//                               ADS1115::Alert_Ready_Polarity::ActiveLow,
//                               ADS1115::LatchingComparator::NonLatching,
//                               ADS1115::AssertStrategy::AssertAfterFourConversion,
//                               0,
//                               0);
//
//        Log::info("AnalogIO", Format("Configure: {1}", Bool(conf)));
//    }
//    else
//    {
//        Log::error("AnalogIO", Format("Not present"));
//    }
}

void G2Alarm::tick()
{
//    uint8_t pin;
//    if (digital_io->read_input(MCP23017::Port::A, pin))
//    {
//        Log::info("DI", Format("{1} {2}", Int32(pin), Bool(digital_io->set_output(MCP23017::Port::B, static_cast<uint8_t>(pin)))));
//    }
}

void G2Alarm::event(const smooth::core::io::InterruptInputEvent& ev)
{
    Log::info("-----", Format("-----"));

    if (ev.get_io() == DIGITAL_CHANGE_PIN)
    {
        Log::info("Digital", Format("kfkf"));
        uint8_t pin;
        if (digital_io->read_interrupt_capture(MCP23017::Port::A, pin))
        {
            Log::info("DI", Format("{1} {2}", Int32(pin), Bool(digital_io->set_output(MCP23017::Port::B, static_cast<uint8_t>(pin)))));
        }
    }
    else if(ev.get_io() == ANALOG_CHANGE_PIN_1)
    {
        Log::info("Analog", Format("asdf"));
    }
}
