//==============================================================================
// Minamoto : Logger Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <queue>

class Logger
{
public:
    static void Create();
    static void Shutdown();
    static void Printf(char const* tag, char const* format, ...);
    static void Update(std::deque<char*>& log);
};
