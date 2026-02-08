//==============================================================================
// Minamoto : InterpolatedTranslateModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier/Modifier.h"

class RuntimeAPI InterpolatedTranslateModifier : public Modifier
{
public:
    struct Key
    {
        float time;
        xxVector3 translate;
    };
    static_assert(sizeof(Key) == 16);

public:
    void                    Update(void* target, float time, xxModifierData* data) override;

    static xxModifierPtr    Create(size_t count = 0, std::function<void(size_t index, float& time, xxVector3& translate)> fill = nullptr);
};
