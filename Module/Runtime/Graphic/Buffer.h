//==============================================================================
// Minamoto : Buffer Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"

struct RuntimeAPI Buffer
{
    static void Initialize();
    static void Update();
    static void Shutdown();
};
