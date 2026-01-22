//==============================================================================
// Minamoto : ImportFilmbox Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Import.h"

struct ImportFilmbox : public Import
{
public:
    static xxNodePtr Create(char const* fbx, ImportCallback callback = nullptr);
};
