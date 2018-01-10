#pragma once

#include <string>

class IAlarmState
{
    public:
        virtual bool is_armed() const = 0;
        virtual void arm(std::string& zone) = 0;
        virtual void disarm() = 0;
        virtual std::string get_current_zone() const = 0;
        virtual void set_current_zone(const std::string& name) = 0;
};