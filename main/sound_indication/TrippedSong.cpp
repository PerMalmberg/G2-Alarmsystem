#include "TrippedSong.h"

TrippedSong::TrippedSong()
        : Song(true)
{
    for(auto i = 0; i < 3; ++i)
    {
        add(Tone(true, std::chrono::milliseconds(150)));
        add(Tone(false, std::chrono::milliseconds(50)));
    }
    add(Tone(false, std::chrono::seconds(5)));
}
