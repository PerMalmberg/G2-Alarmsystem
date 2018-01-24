#pragma once

#include <chrono>
#include <vector>

typedef std::pair<bool, std::chrono::milliseconds> Tone;
typedef std::vector<Tone> Melody;

class Song
{
    public:

        Song() = default;

        explicit Song(bool repeat) : repeat(repeat)
        {
        }

        void add(const Tone& note)
        {
            melody.emplace_back(note);
        }

        const Melody& get_melody() const
        {
            return melody;
        };

        bool is_repeating() const
        {
            return repeat;
        }

    private:
        Melody melody{};
        bool repeat = false;
};
