//==============================================================================
// Minamoto : Event Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <queue>
#include "Event.h"

//==============================================================================
//  Event
//==============================================================================
static std::priority_queue<std::pair<double, std::shared_ptr<Event>>> events;
static std::vector<std::pair<double, std::shared_ptr<Event>>> continues;
//------------------------------------------------------------------------------
void Event::Dispatch(double time)
{
    while (events.empty() == false)
    {
        auto const& pair = events.top();
        if (pair.first > time)
            break;

        auto const& event = pair.second;
        double next = event->Execute();
        if (next > 0.0f)
        {
            continues.emplace_back(next, event);
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
    double time;
    xxGetCurrentTime(&time);
    events.push({ time, event });
}
//==============================================================================
