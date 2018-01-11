#pragma once

#include <smooth/core/fsm/StaticFSM.h>
#include <smooth/core/Task.h>
#include "IOStatus.h"
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/application/rgb_led/RGBLed.h>
#include <smooth/core/io/Output.h>
#include <Config.h>
#include "states/events/DigitalValueNotIdle.h"
#include "states/events/AnalogValueOutsideLimits.h"

template<typename BaseState>
class AlarmFSM
        : public smooth::core::fsm::StaticFSM<BaseState, 100>,
          smooth::core::ipc::IEventListener<AnalogValueOutsideLimits>,
          smooth::core::ipc::IEventListener<DigitalValueNotIdle>
{
    public:
        explicit AlarmFSM(IOStatus& io_status, Config& cfg, smooth::core::Task& task);

        void tick();
        void arm(const std::string& zone_name);
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

        std::chrono::seconds get_exit_delay() const;

        bool is_armed() const;
        bool is_arming() const;

        bool is_input_enabled(const std::string& name);
    private:
        IOStatus& io_status;
        Config& cfg;
        smooth::core::Task& task;
        smooth::core::ipc::SubscribingTaskEventQueue<AnalogValueOutsideLimits> analog_events;
        smooth::core::ipc::SubscribingTaskEventQueue<DigitalValueNotIdle> digital_events;
        smooth::application::rgb_led::RGBLed rgb;
        smooth::core::io::Output bell;
};

template<typename BaseState>
void AlarmFSM<BaseState>::arm(const std::string& zone_name)
{
    this->get_state()->arm();
}

template<typename BaseState>
AlarmFSM<BaseState>::AlarmFSM(IOStatus& io_status, Config& cfg, smooth::core::Task& task)
        : io_status(io_status),
          cfg(cfg),
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

template<typename BaseState>
bool AlarmFSM<BaseState>::is_armed() const
{
    return this->get_state()->is_armed();
}

template<typename BaseState>
bool AlarmFSM<BaseState>::is_arming() const
{
    return this->get_state()->is_arming();
}

template<typename BaseState>
std::chrono::seconds AlarmFSM<BaseState>::get_exit_delay() const
{
    return cfg.get_exit_delay();
}

template<typename BaseState>
bool AlarmFSM<BaseState>::is_input_enabled(const std::string& name)
{
    return cfg.is_input_enabled(name);
}

