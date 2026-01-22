//==============================================================================
// Minamoto : ExportFilmbox Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <map>

struct ExportFilmbox
{
public:
    static bool Save(char const* path, xxNodePtr const& root);
};
