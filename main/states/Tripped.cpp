//
// Created by permal on 12/28/17.
//

#include <chrono>
#include <I2CSetOutputCmd.h>
#include <smooth/core/ipc/Publisher.h>
#include "Tripped.h"
#include "Idle.h"
#include <sound_indication/TrippedSong.h>
#include <sound_indication/Silence.h>
#include "OutputNumber.h"

using namespace smooth::core::ipc;
using namespace std::chrono;

void Tripped::enter_state()
{
    I2CSetOutput s(SIREN, true);
    Publisher<I2CSetOutput>::publish(s);

    max_time.start();
    Publisher<Song>::publish(TrippedSong());
}

void Tripped::leave_state()
{

    Publisher<Song>::publish(Silence());

    I2CSetOutput s(SIREN, false);
    Publisher<I2CSetOutput>::publish(s);
}

void Tripped::disarm()
{
    fsm.set_state(new(fsm)Idle(fsm));
}

void Tripped::tick()
{
    roll_color(33, 0, 0);

    if(max_time.get_running_time() > fsm.get_tripped_max_time())
    {
        fsm.set_state(new(fsm) Idle(fsm));
    }
}
