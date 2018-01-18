#include "ErrorSong.h"

ErrorSong::ErrorSong()
        : Song(false)
{
    add(Tone(true, std::chrono::seconds(1)));
}
