//==============================================================================
// Minamoto : QuickJSConsole Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

class QuickJSConsole
{
public:
    static void Initialize();
    static void Shutdown();
    static bool Update(const UpdateData& updateData, bool& show);
};
