
#include "Armed.h"
#include "Idle.h"
#include "Tripped.h"
#include <chrono>
#include <smooth/core/ipc/Publisher.h>
#include <sound_indication/DisarmSong.h>
#include <sound_indication/EntryDelaySong.h>
#include <sound_indication/Silence.h>

using namespace smooth::core::logging;
using namespace smooth::core::ipc;
using namespace std::chrono;

void Armed::enter_state()
{
    Publisher<Song>::publish(Silence());
}

void Armed::leave_state()
{
}

void Armed::disarm()
{
    Publisher<Song>::publish(DisarmSong());
    fsm.set_state(new(fsm)Idle(fsm));
}

void Armed::event(const AnalogValueOutsideLimits& event)
{
    trip(event.get_name());
}

void Armed::event(const DigitalValueNotIdle& event)
{
    trip(event.get_name());
}

void Armed::tick()
{
    fsm.clear_rgb();
    if (entry_delay.is_running())
    {
        roll_color(33, 33, 33);
    }
    else
    {
        roll_color(33, 33, 0);
    }

    if (entry_delay.get_running_time() > entry_delay_limit)
    {
        fsm.set_state(new(fsm)Tripped(fsm));
    }
}

void Armed::trip(const std::string& input_name)
{
    if (!entry_delay.is_running())
    {
        if (fsm.is_input_enabled(input_name))
        {
            Publisher<Song>::publish(EntryDelaySong());
            entry_delay_limit = fsm.get_entry_delay(input_name);
            entry_delay.start();
        }
    }
}
