//
// Created by permal on 11/24/17.
//

#include "I2CTask.h"
#include <smooth/core/util/ByteSet.h>
#include <smooth/core/ipc/Publisher.h>
#include "AnalogValue.h"
#include "DigitalValue.h"
#include <smooth/core/util/ByteSet.h>

using namespace std::chrono;
using namespace smooth::core::io;
using namespace smooth::core::logging;
using namespace smooth::core::util;
using namespace smooth::application::io;
using namespace smooth::core::ipc;

static const gpio_num_t DIGITAL_CHANGE_PIN = GPIO_NUM_33;
static const gpio_num_t ANALOG_CHANGE_PIN_1 = GPIO_NUM_34;
static const gpio_num_t ANALOG_CHANGE_PIN_2 = GPIO_NUM_35;

I2CTask::I2CTask()
        : Task("I2CTask", 8096, 5, milliseconds(100)),
          i2c_power(GPIO_NUM_27, true, false, true),
          digital_i2c_master(I2C_NUM_0, GPIO_NUM_16, false, GPIO_NUM_17, false, 100000),
          analog_i2c_master(I2C_NUM_1, GPIO_NUM_25, false, GPIO_NUM_26, false, 100000),
          input_change_queue(*this, *this),
          analog_change_queue_1(*this, *this),
          analog_change_queue_2(*this, *this),
          digital_input_change(input_change_queue, DIGITAL_CHANGE_PIN, false, false, GPIO_INTR_ANYEDGE),
          analog_change_1(analog_change_queue_1, ANALOG_CHANGE_PIN_1, false, false, GPIO_INTR_NEGEDGE),
          analog_change_2(analog_change_queue_2, ANALOG_CHANGE_PIN_2, false, false, GPIO_INTR_NEGEDGE),
          set_output_cmd("set_output_cmd", 10, *this, *this)
{
}

void I2CTask::init()
{
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

        digital_io->set_output(MCP23017::Port::B, 0);
    }
    else
    {
        Log::error("DigitalIO", Format("Not present"));
    }

    cycler_1 = make_unique<AnalogCycler>(analog_i2c_master.create_device<ADS1115>(0x48));
    cycler_2 = make_unique<AnalogCycler>(analog_i2c_master.create_device<ADS1115>(0x49));

    // Read inputs once on startup to clear waiting interrupts on the i2c devices.
    update_inputs();
}

void I2CTask::tick()
{
    cycler_1->trigger_read();
    cycler_2->trigger_read();
}

void I2CTask::event(const smooth::core::io::InterruptInputEvent& ev)
{
    if (ev.get_io() == DIGITAL_CHANGE_PIN)
    {
        uint8_t pins;
        if (digital_io->read_interrupt_capture(MCP23017::Port::A, pins))
        {
            publish_digital(pins);
        }
    }
    else if (ev.get_io() == ANALOG_CHANGE_PIN_1)
    {
        uint16_t result = cycler_1->get_value();

        AnalogValue av(10 + cycler_1->get_input_number(), result);
        Publisher<AnalogValue>::publish(av);

        cycler_1->cycle();
    }
    else if (ev.get_io() == ANALOG_CHANGE_PIN_2)
    {
        uint16_t result = cycler_2->get_value();
        AnalogValue av(20 + cycler_2->get_input_number(), result);
        Publisher<AnalogValue>::publish(av);

        cycler_2->cycle();
    }
}

void I2CTask::update_inputs()
{
    uint8_t digital;
    digital_io->read_input(MCP23017::Port::A, digital);
    publish_digital(digital);
    cycler_1->cycle();
    cycler_2->cycle();
}

void I2CTask::publish_digital(uint8_t pins)
{

    for (uint8_t i = 0; i < 8; ++i)
    {
        DigitalValue dv(i, static_cast<bool>(pins & 1));
        Publisher<DigitalValue>::publish(dv);
        pins >>= 1;
    }
}

void I2CTask::event(const I2CSetOutput& ev)
{
    smooth::core::util::ByteSet b(output_state);
    b.set(ev.get_io(), ev.get_state());
    digital_io->set_output(smooth::application::io::MCP23017::Port::B, b);
}
