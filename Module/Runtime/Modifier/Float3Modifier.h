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

public:
    void                    Assign(void* source, float time) override;
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(xxVector3 const& value = xxVector3::ZERO);
};
