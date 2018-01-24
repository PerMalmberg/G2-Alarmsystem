//
// Created by permal on 1/18/18.
//

#include "EntryDelaySong.h"
#include <chrono>

using namespace std::chrono;

EntryDelaySong::EntryDelaySong()
        : Song(true)
{
    add(Tone(true, milliseconds(300)));
    add(Tone(false, milliseconds(300)));
}
