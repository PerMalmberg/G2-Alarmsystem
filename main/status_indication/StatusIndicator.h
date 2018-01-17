#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/io/Output.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/SubscribingTaskEventQueue.h>
#include <smooth/core/timer/TimerExpiredEvent.h>
#include <smooth/core/timer/Timer.h>
#include "Song.h"

class StatusIndicator
        : public smooth::core::Task,
        smooth::core::ipc::IEventListener<Song>,
        smooth::core::ipc::IEventListener<smooth::core::timer::TimerExpiredEvent>
{
    public:
        StatusIndicator();
        void event(const Song& new_song) override;
        void play_next_tone();
        void event(const smooth::core::timer::TimerExpiredEvent& event);

    private:
        smooth::core::io::Output bell;
        Song song;
        Melody::const_iterator current_tone{};
        smooth::core::ipc::SubscribingTaskEventQueue<Song> song_to_play;
        smooth::core::ipc::TaskEventQueue<smooth::core::timer::TimerExpiredEvent> tone_complete;
        std::shared_ptr<smooth::core::timer::Timer> playing_tone{};

        void play(Melody::const_iterator& tone);
};
