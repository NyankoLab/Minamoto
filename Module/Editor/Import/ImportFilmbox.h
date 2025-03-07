//==============================================================================
// Minamoto : ImportFilmbox Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Import.h"

struct ImportFilmbox : public Import
{
public:
    static xxNodePtr Create(char const* fbx, ImportCallback callback = nullptr);
};
