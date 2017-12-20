#pragma once

#include <string>
#include <unordered_map>
#include <functional>


typedef std::function<void(const std::string&)> Executor;

class CommandDispatcher
{
    public:
        void add_command(const std::string& command_topic, Executor executor);
        void process(const std::string& topic, const std::string& data);
    private:
        std::unordered_map<std::string, Executor> execs{};
};

