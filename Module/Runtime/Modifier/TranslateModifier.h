//==============================================================================
// Minamoto : TranslateModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI TranslateModifier : public Modifier
{
public:
    struct Key
    {
        float time;
        xxVector3 translate;
    };
    static_assert(sizeof(Key) == 16);

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create(size_t count = 0, std::function<void(size_t index, float& time, xxVector3& translate)> fill = nullptr);
};
