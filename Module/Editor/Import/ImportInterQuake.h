//==============================================================================
// Minamoto : ImportInterQuake Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <map>
#include "Import.h"

struct ImportInterQuake : public Import
{
public:
    static xxNodePtr Create(char const* iqe, std::function<void(xxNodePtr&&)> callback = nullptr);
};
