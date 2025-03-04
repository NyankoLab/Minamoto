//==============================================================================
// Minamoto : ImportInterQuake Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <map>
#include "Import.h"

struct ImportInterQuake : public Import
{
public:
    static xxNodePtr Create(char const* iqe, std::function<void(xxNodePtr&&)> callback = nullptr);
};
