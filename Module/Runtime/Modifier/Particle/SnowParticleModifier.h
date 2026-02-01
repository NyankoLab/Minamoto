//==============================================================================
// Minamoto : SnowParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "ParticleModifier.h"

class RuntimeAPI SnowParticleModifier : public ParticleModifier
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
        xxVector4 bound;

        void CalculateBound();
    };
    static_assert(sizeof(Parameter) == 56);

    struct Header
    {
        int seed;
    };

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create();
};
