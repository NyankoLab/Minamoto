//==============================================================================
// Minamoto : Event Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

struct Event
{
    virtual ~Event() {}
    virtual float Execute() = 0;
    static void Dispatch(float time);
    static void Push(std::shared_ptr<Event> const& event);
};
