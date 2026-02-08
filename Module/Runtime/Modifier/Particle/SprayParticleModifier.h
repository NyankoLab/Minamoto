//==============================================================================
// Minamoto : SnowParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#if HAVE_PARTICLE

#include "ParticleModifier.h"

class RuntimeAPI SprayParticleModifier : public ParticleModifier
{
public:
    struct Parameter
    {
        int now;
        int count;
        float size;
        float speed;
        float variation;
        float start;
        float life;
        float birth;
        float tumble;
        float scale;
        xxVector2 range;
        xxVector4 bound;

    public:
        void RuntimeAPI CalculateBound();
    };
    static_assert(sizeof(Parameter) == 64);

    struct Header
    {
        int seed;
        xxModifierData modifiers[8];
        Particle particles[];
    };

public:
    void                    Update(void* target, float time, xxModifierData* data);

    static xxModifierPtr    Create();
};

#endif
