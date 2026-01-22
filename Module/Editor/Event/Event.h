//==============================================================================
// Minamoto : Event Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Event
{
    virtual ~Event() {}
    virtual double Execute() = 0;
    static void Dispatch(double time);
    static void Push(std::shared_ptr<Event> const& event);
};
