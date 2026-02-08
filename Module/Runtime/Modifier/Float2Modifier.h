//==============================================================================
// Minamoto : Float2Modifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI Float2Modifier : public Modifier
{
public:
    void                    Set(xxVector2 const& value);
    xxVector2               Get() const;

public:
    void                    Assign(void* source, float time) override;
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(xxVector2 const& value = xxVector2::ZERO);
};
