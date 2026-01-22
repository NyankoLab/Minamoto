//==============================================================================
// Minamoto : ConstantTranslateModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ConstantTranslateModifier : public Modifier
{
public:
    struct Constant
    {
        xxVector3 translate;
    };

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(xxVector3 const& translate = xxVector3::ZERO);
};
