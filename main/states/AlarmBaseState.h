#pragma once

#include "AlarmFSM.h"

class AlarmBaseState
{
    public:
        explicit AlarmBaseState(AlarmFSM<AlarmBaseState>& fsm, const std::string& name) : fsm(fsm), name(name)
        {
        }

        virtual void enter_state()
        {
        }

        virtual void leave_state()
        {
        }

        virtual void arm()
        {
        }

        virtual void disarm()
        {
        }

        virtual void tick()
        {
        }

        virtual bool is_armed() const
        {
            return false;
        }

        virtual bool is_arming() const
        {
            return false;
        }

        std::string get_name() const
        {
            return name;
        }

        virtual void event(const AnalogValueOutsideLimits& event)
        {
        }

        virtual void event(const DigitalValueNotIdle& event)
        {
        }

    protected:

        void roll_color(uint8_t r, uint8_t g, uint8_t b)
        {
            fsm.clear_rgb();
            fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 0, 33, 0);
            if(fsm.is_network_connected())
            {
                fsm.set_pixel(static_cast<uint16_t>(4), 0, 33, 0);
            }
            else
            {
                fsm.set_pixel(static_cast<uint16_t>(4), 33, 0, 0);
            }

            if(fsm.is_mqtt_connected())
            {
                fsm.set_pixel(static_cast<uint16_t>(3), 0, 33, 0);
            }
            else
            {
                fsm.set_pixel(static_cast<uint16_t>(3), 33, 0, 0);
            }

            fsm.apply_rgb();
        }

        AlarmFSM<AlarmBaseState>& fsm;
    private:
        std::string name;
        uint16_t led = 0;
};