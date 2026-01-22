//==============================================================================
// Minamoto : ImportPolygon Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <map>
#include "Import.h"

struct ImportPolygon : public Import
{
public:
    static xxNodePtr Create(char const* ply);
};
