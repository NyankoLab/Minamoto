//==============================================================================
// Minamoto : Module Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Interface.h"

class Module
{
public:
    static void Create(char const* path, uint64_t device);
    static void Shutdown();

    static int Count();
    static void Message(std::initializer_list<char const*> list);
    static bool Update();
    static void Render();
};
