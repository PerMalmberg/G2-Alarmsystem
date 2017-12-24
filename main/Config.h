#pragma once

#include <string>
#include <unordered_map>

class Config
{
    public:
        bool parse(const std::string& data);
    private:
        std::unordered_map<std::string,std::string> io_names{};
};
