//==============================================================================
// Minamoto : ExportWavefront Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <map>

struct ExportWavefront
{
public:
    static bool Save(char const* path, xxNodePtr const& node);
};
