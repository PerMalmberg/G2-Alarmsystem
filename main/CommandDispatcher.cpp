//
// Created by permal on 12/20/17.
//

#include "CommandDispatcher.h"


void CommandDispatcher::process(const std::string& topic, const std::string& data)
{
    auto it = execs.find(topic);
    if( it != execs.end())
    {
        it->second(data);
    }
}

void CommandDispatcher::add_command(const std::string& command_topic, Executor executor)
{
    execs.emplace(command_topic, executor);
}
