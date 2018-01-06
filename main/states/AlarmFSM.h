#pragma once

#include <smooth/core/fsm/StaticFSM.h>
#include <smooth/core/Task.h>
#include "IOStatus.h"
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/application/rgb_led/RGBLed.h>
#include <smooth/core/io/Output.h>
#include "states/events/DigitalValueNotIdle.h"
#include "states/events/AnalogValueOutsideLimits.h"

template<typename BaseState>
class AlarmFSM
        : public smooth::core::fsm::StaticFSM<BaseState, 100>,
          smooth::core::ipc::IEventListener<AnalogValueOutsideLimits>,
          smooth::core::ipc::IEventListener<DigitalValueNotIdle>
{
    public:
        explicit AlarmFSM(IOStatus& io_status, smooth::core::Task& task);

        void tick();
        void arm();
        bool are_all_inputs_within_limits() const;
        void disarm();

        void event(const AnalogValueOutsideLimits& event);
        void event(const DigitalValueNotIdle& event);

        void set_pixel(uint16_t ix, uint8_t red, uint8_t green, uint8_t blue)
        {
            rgb.set_pixel(ix, red, green, blue);
        }

        void apply_rgb()
        {
            rgb.apply();
        }

        void clear_rgb()
        {
            rgb.clear();
        }

        const std::string get_state_name();
    private:
        IOStatus& io_status;
        smooth::core::Task& task;
        smooth::core::ipc::SubscribingTaskEventQueue<AnalogValueOutsideLimits> analog_events;
        smooth::core::ipc::SubscribingTaskEventQueue<DigitalValueNotIdle> digital_events;
        smooth::application::rgb_led::RGBLed rgb;
        smooth::core::io::Output bell;

};

template<typename BaseState>
void AlarmFSM<BaseState>::arm()
{
    this->get_state()->arm();
}

template<typename BaseState>
AlarmFSM<BaseState>::AlarmFSM(IOStatus& io_status, smooth::core::Task& task)
        : io_status(io_status),
          task(task),
          analog_events("FSMAnalog", 10, task, *this),
          digital_events("FSMDigital", 10, task, *this),
          rgb(RMT_CHANNEL_0, GPIO_NUM_2, 5, smooth::application::rgb_led::WS2812B()),
          bell(GPIO_NUM_32, true, false, true, true)
{
}

template<typename BaseState>
bool AlarmFSM<BaseState>::are_all_inputs_within_limits() const
{
    return io_status.are_all_inputs_within_limits();
}

template<typename BaseState>
void AlarmFSM<BaseState>::disarm()
{
    this->get_state()->disarm();
}

template<typename BaseState>
void AlarmFSM<BaseState>::tick()
{
    this->get_state()->tick();
}

template<typename BaseState>
void AlarmFSM<BaseState>::event(const AnalogValueOutsideLimits& event)
{
    this->get_state()->event(event);
}

template<typename BaseState>
void AlarmFSM<BaseState>::event(const DigitalValueNotIdle& event)
{
    this->get_state()->event(event);
}

template<typename BaseState>
const std::string AlarmFSM<BaseState>::get_state_name()
{
    return this->get_state()->get_name();
}
