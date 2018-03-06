#include "DisarmSong.h"

#include <chrono>

using namespace std::chrono;

DisarmSong::DisarmSong()
        : Song(false)
{
    for(int i = 0; i < 3; ++i)
    {
        add(Tone(true, milliseconds(30)));
        add(Tone(false, milliseconds(30)));
    }
}
