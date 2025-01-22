//==============================================================================
// Minamoto : FloatModifier Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI FloatModifier : public Modifier
{
public:
    void                    Set(float value);
    float                   Get() const;

    static xxModifierPtr    Create(float value = 0.0f);
};
