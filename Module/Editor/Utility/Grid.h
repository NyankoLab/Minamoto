//==============================================================================
// Minamoto : Grid Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct Grid
{
    static xxNodePtr Create(xxVector3 const& translate, xxVector2 const& size);
    static xxTexturePtr CreateTexture();
};
