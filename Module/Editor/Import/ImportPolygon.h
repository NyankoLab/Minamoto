//==============================================================================
// Minamoto : ImportPolygon Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <map>
#include "Import.h"

struct ImportPolygon : public Import
{
public:
    static xxNodePtr Create(char const* ply);
};
