#pragma once

#include <string>
#include <vector>
#include "AnalogRef.h"

class IReferenceValueGetter
{
    public:
        virtual ~IReferenceValueGetter() = default;

        virtual const AnalogRef& get_analog_reference(const std::string& short_name) = 0;
        virtual bool get_digital_idle(const std::string& short_name) = 0;
        virtual bool get_digital_startup_state(const std::string& short_name) = 0;
        virtual bool is_input_enabled(const std::string& short_name) = 0;
        virtual const std::vector<std::string> get_digital_input_names() = 0;
        virtual const std::vector<std::string> get_analog_input_names() = 0;

};