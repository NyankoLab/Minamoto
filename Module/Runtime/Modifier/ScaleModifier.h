//==============================================================================
// Minamoto : ScaleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ScaleModifier : public Modifier
{
public:
    struct Key
    {
        float time;
        float scale;
    };
    static_assert(sizeof(Key) == 8);

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(size_t count = 0, std::function<void(size_t index, float& time, float& scale)> fill = nullptr);
};
