//==============================================================================
// Minamoto : Setup Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Setup
{
    static void Initialize();
    static void Shutdown();
    static void Load();
    static void Save();
    static bool Update(const UpdateData& updateData, bool& show);
};
