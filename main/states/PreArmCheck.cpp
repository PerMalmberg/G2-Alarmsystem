#include "PreArmCheck.h"
#include "Idle.h"
#include "Armed.h"
#include <smooth/core/ipc/Publisher.h>
#include <status_indication/TrippedSong.h>
#include <status_indication/ErrorSong.h>
#include <status_indication/DisarmSong.h>
#include <status_indication/ExitDelaySong.h>

using namespace smooth::core::ipc;

void PreArmCheck::enter_state()
{
    // Check if all inputs are within limits
    if (fsm.are_all_inputs_within_limits())
    {
        Publisher<Song>::publish(ExitDelaySong());
        elapsed.start();
    }
    else
    {
        Publisher<Song>::publish(ErrorSong());
        fsm.set_state(new(fsm) Idle(fsm));
    }
}

void PreArmCheck::tick()
{
    if (elapsed.get_running_time() > fsm.get_exit_delay())
    {
        if (fsm.are_all_inputs_within_limits())
        {
            fsm.set_state(new(fsm) Armed(fsm));
        }
        else
        {
            Publisher<Song>::publish(ErrorSong());
            fsm.set_state(new(fsm) Idle(fsm));
        }
    }

    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 0, 33, 33);
    fsm.apply_rgb();
}

void PreArmCheck::disarm()
{
    Publisher<Song>::publish(DisarmSong());
    fsm.set_state(new(fsm) Idle(fsm));
}
