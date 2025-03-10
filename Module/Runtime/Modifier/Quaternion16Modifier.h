//==============================================================================
// Minamoto : Quaternion16Modifier Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI Quaternion16Modifier : public Modifier
{
public:
    struct Key
    {
        float time;
#if defined(_M_ARM) || defined(_M_ARM64) || defined(_M_IX86) || defined(_M_AMD64) 
        int16_t quaternion[4];
#else
        v4hi quaternion;
#endif
    };
    static_assert(sizeof(Key) == 12);

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(size_t count = 0, std::function<void(size_t index, float& time, xxVector4& quaternion)> fill = nullptr);
};
