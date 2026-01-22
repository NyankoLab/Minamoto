//==============================================================================
// Minamoto : Float3Modifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI Float3Modifier : public Modifier
{
public:
    void                    Set(xxVector3 const& value);
    xxVector3               Get() const;

    static xxModifierPtr    Create(xxVector3 const& value = xxVector3::ZERO);
};
