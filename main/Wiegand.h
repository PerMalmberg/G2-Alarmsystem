#pragma once

#include <smooth/core/Task.h>
#include <smooth/core/io/InterruptInput.h>
#include <smooth/core/ipc/IEventListener.h>
#include <smooth/core/ipc/ISRTaskEventQueue.h>
#include <smooth/core/ipc/ITaskEventQueue.h>

class Wiegand : public smooth::core::ipc::IEventListener<smooth::core::io::InterruptInputEvent>
{
    public:
        Wiegand(smooth::core::Task& task);
        void event(const smooth::core::io::InterruptInputEvent& event) override;

    private:
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 20> d1_queue;
        smooth::core::ipc::ISRTaskEventQueue<smooth::core::io::InterruptInputEvent, 20> d2_queue;
        smooth::core::io::InterruptInput d1;
        smooth::core::io::InterruptInput d2;
};
