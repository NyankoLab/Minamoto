//==============================================================================
// Minamoto : ConstantScaleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ScaleModifier : public Modifier
{
public:
    struct Constant
    {
        float scale;
    };

public:
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(float scale = 1.0f);
};
