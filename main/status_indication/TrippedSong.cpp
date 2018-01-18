#include "TrippedSong.h"

TrippedSong::TrippedSong()
        : Song(true)
{
    add(Tone(true, std::chrono::milliseconds(150)));
    add(Tone(false, std::chrono::milliseconds(50)));
}
