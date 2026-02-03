//==============================================================================
// Minamoto : SnowParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#if HAVE_PARTICLE

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
        float tumble;
        float scale;
        xxVector4 bound;

    public:
        void CalculateBound();
    };
    static_assert(sizeof(Parameter) == 64);

    struct Header
    {
        int seed;
        Particle particles[];
    };
    static_assert(sizeof(Header) == 4);

public:
    void                    Update(void* target, xxModifierData* data, float time);

    static xxModifierPtr    Create();
};

#endif
