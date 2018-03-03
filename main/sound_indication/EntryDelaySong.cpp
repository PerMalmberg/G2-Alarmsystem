//
// Created by permal on 1/18/18.
//

#include "EntryDelaySong.h"
#include <chrono>

using namespace std::chrono;

EntryDelaySong::EntryDelaySong()
        : Song(true)
{
    for (auto i = 0; i < 2; ++i) {
        add(Tone(true, milliseconds(500)));
        add(Tone(false, milliseconds(500)));
    }
    add(Tone(false, seconds(10)));
}