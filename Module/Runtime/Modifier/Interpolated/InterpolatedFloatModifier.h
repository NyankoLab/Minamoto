//==============================================================================
// Minamoto : InterpolatedFloatModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier/Modifier.h"

class RuntimeAPI InterpolatedFloatModifier : public Modifier
{
public:
    struct Key
    {
        float time;
        float value;
    };
    static_assert(sizeof(Key) == 8);

public:
    void                    Assign(void* source, float time) override;
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(size_t count = 0, std::function<void(size_t index, float& time, float& value)> fill = nullptr);
};
