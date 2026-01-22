//==============================================================================
// Minamoto : ConstantScaleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ConstantScaleModifier : public Modifier
{
public:
    struct Constant
    {
        float scale;
    };

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(float scale = 1.0f);
};
