//==============================================================================
// Minamoto : ExportFilmbox Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <map>

struct ExportFilmbox
{
public:
    static bool Save(char const* path, xxNodePtr const& node);
};
