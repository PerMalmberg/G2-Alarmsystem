//
// Created by permal on 12/28/17.
//

#include <I2CSetOutputCmd.h>
#include <smooth/core/ipc/Publisher.h>
#include <I2CSetOutputCmd.h>
#include "Tripped.h"
#include "Idle.h"
#include <status_indication/Song.h>
#include <chrono>

using namespace smooth::core::ipc;
using namespace std::chrono;

void Tripped::enter_state()
{
    I2CSetOutput s(0, true);
    Publisher<I2CSetOutput>::publish(s);
    max_time.start();
    Song song{true};
    song.add(Tone(true, milliseconds(150)));
    song.add(Tone(false, milliseconds(50)));

    Publisher<Song>::publish(song);
}

void Tripped::leave_state()
{
    I2CSetOutput s(0, false);

    Song song{false};
    Publisher<Song>::publish(song);

    Publisher<I2CSetOutput>::publish(s);
}

void Tripped::disarm()
{
    fsm.set_state(new(fsm)Idle(fsm));
}

void Tripped::tick()
{
    fsm.clear_rgb();
    fsm.set_pixel(static_cast<uint16_t>(led++ % 5), 33, 0, 0);
    fsm.apply_rgb();

    if(max_time.get_running_time() > fsm.get_tripped_max_time())
    {
        fsm.set_state(new(fsm) Idle(fsm));
    }
}
