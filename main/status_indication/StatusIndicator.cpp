#include "StatusIndicator.h"
#include <chrono>
#include <smooth/core/task_priorities.h>

using namespace smooth::core::timer;
using namespace std::chrono;

StatusIndicator::StatusIndicator() :
        Task("StatusIndicator", 8092, smooth::core::APPLICATION_BASE_PRIO, std::chrono::seconds(10)),
        bell(GPIO_NUM_32, true, false, true, true),
        song(false),
        current_tone(song.get_melody().end()),
        song_to_play("song_to_play", 5, *this, *this),
        tone_complete("tone_complete", 5, *this, *this)
{
    playing_tone = Timer::create("Playing", 1, tone_complete, false, seconds(1));
}

void StatusIndicator::event(const Song& new_song)
{
    // New song to play
    song = new_song;
    current_tone = song.get_melody().begin();
    play_next_tone();
}

void StatusIndicator::play(Melody::const_iterator& tone)
{
    auto& t = *tone;
    if(t.first)
    {
        bell.set();
    }
    else
    {
        bell.clr();
    }

    current_tone++;
    playing_tone->start(t.second);
}

void StatusIndicator::play_next_tone()
{
    const auto& melody = song.get_melody();

    if(!melody.empty())
    {
        if (current_tone == melody.end())
        {
            if(song.is_repeating())
            {
                current_tone = melody.begin();
                play(current_tone);
            }
            else
            {
                bell.clr();
            }
        }
        else
        {
            play(current_tone);
        }
    }
    else
    {
        bell.clr();
    }
}

void StatusIndicator::event(const TimerExpiredEvent&)
{
    play_next_tone();
}
