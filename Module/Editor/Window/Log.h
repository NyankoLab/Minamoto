//==============================================================================
// Minamoto : Log Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Log
{
    static void Initialize();
    static void Shutdown();
    static bool StatusBar();
    static bool Update(const UpdateData& updateData, bool& show);
};
