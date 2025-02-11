//==============================================================================
// Minamoto : ScaleModifier Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
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
