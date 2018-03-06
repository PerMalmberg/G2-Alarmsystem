#include <sound_indication/Song.h>
#include <sound_indication/Silence.h>
#include <smooth/core/ipc/Publisher.h>
#include "Idle.h"
#include "PreArmCheck.h"

using namespace smooth::core::ipc;

void Idle::arm()
{
    fsm.set_state(new(fsm) PreArmCheck(fsm));
}

void Idle::enter_state()
{
}

void Idle::tick()
{
    roll_color(0, 33, 0);
}