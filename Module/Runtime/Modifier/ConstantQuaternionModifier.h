//==============================================================================
// Minamoto : ConstantQuaternionModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ConstantQuaternionModifier : public Modifier
{
public:
    struct Constant
    {
        xxVector4 quaternion;
    };

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(xxVector4 const& quaternion = xxVector4::W);
};
