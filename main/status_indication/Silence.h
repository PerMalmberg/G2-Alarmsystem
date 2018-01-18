#pragma once

#include "Song.h"
#include <chrono>

class Silence : public Song
{
    public:
        Silence() : Song(false)
        {}
};
