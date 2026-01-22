//==============================================================================
// Minamoto : Float4Modifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI Float4Modifier : public Modifier
{
public:
    void                    Set(xxVector4 const& value);
    xxVector4               Get() const;

    static xxModifierPtr    Create(xxVector4 const& value = xxVector4::ZERO);
};
