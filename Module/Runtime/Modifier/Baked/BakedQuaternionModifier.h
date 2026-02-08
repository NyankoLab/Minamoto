//==============================================================================
// Minamoto : BakedQuaternionModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier/Modifier.h"

class RuntimeAPI BakedQuaternionModifier : public Modifier
{
public:
    struct Baked
    {
        float duration;
        float frequency;
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64) 
        float values[][4];
#else
        xxVector4 values[];
#endif
    };
    static_assert(sizeof(Baked) == 8);

public:
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(size_t count = 0, float duration = 0.0f, std::function<void(size_t index, xxVector4& quaternion)> fill = nullptr);
};
