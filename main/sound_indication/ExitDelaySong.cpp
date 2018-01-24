#include "ExitDelaySong.h"

#include <chrono>

using namespace std::chrono;

ExitDelaySong::ExitDelaySong()
        : Song(true)
{
    add(Tone(true, milliseconds(50)));
    add(Tone(false, milliseconds(500)));
}
