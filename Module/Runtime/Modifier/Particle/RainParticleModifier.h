//==============================================================================
// Minamoto : RainParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "ParticleModifier.h"

class RuntimeAPI RainParticleModifier : public ParticleModifier
{
public:
    struct Parameter
    {
        int now;
        int count;
        float start;
        float birth;
        float life;
        float width;
        float height;
        float size;
        float speed;
        float variation;
    };
    static_assert(sizeof(Parameter) == 40);

    struct Header
    {
        xxVector4 bound;
        int seed;
    };

public:
    void                    Update(void* target, xxModifierData* data, float time);
    void                    CalculateBound(xxModifierData* data);

    static xxModifierPtr    Create();
};
