//==============================================================================
// Minamoto : Event Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <queue>
#include "Event.h"

//==============================================================================
//  Event
//==============================================================================
static std::priority_queue<std::pair<float, std::shared_ptr<Event>>> events;
//------------------------------------------------------------------------------
void Event::Dispatch(float time)
{
    static std::vector<std::pair<float, std::shared_ptr<Event>>> continues;

    while (events.empty() == false && events.top().first < time)
    {
        float next = events.top().second->Execute();
        if (next >= time)
        {
            continues.push_back({ next, events.top().second });
        }
        events.pop();
    }

    for (auto pair : continues)
        events.push(pair);
    continues.clear();
}
//------------------------------------------------------------------------------
void Event::Push(std::shared_ptr<Event> const& event)
{
    events.push({ xxGetCurrentTime(), event });
}
//==============================================================================
