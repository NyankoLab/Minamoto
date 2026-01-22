//==============================================================================
// Minamoto : Document Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Document
{
    std::function<bool(UpdateData const&, std::string const&)> Callback;
    std::string File;
    std::string Text;
    std::string Title;

    static void Initialize();
    static void Shutdown();
    static bool Update(UpdateData const& updateData);

    static bool OpenFile(char const* name, std::function<bool(UpdateData const&, std::string const&)> callback);
};
