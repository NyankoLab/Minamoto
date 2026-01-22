//==============================================================================
// Minamoto : Game Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

class Game
{
public:
    static void Initialize();
    static void Shutdown(bool suspend = false);
    static bool Update(const UpdateData& updateData, bool& show);
    static void Callback(const ImDrawList* list, const ImDrawCmd* cmd);
};
