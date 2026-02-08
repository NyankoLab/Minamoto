//==============================================================================
// Minamoto : BakedQuaternion16Modifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier/Modifier.h"

class RuntimeAPI BakedQuaternion16Modifier : public Modifier
{
public:
    struct Baked
    {
        float duration;
        float frequency;
        float inverseFrequency;
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64) 
        int16_t values[][4];
#else
        v4hi values[];
#endif
    };
    static_assert(sizeof(Baked) == 12);

public:
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(size_t count = 0, float duration = 0.0f, std::function<void(size_t index, xxVector4& quaternion)> fill = nullptr);
};
