//==============================================================================
// Minamoto : SuperSprayParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#if HAVE_PARTICLE

#include "ParticleModifier.h"

class RuntimeAPI SuperSprayParticleModifier : public ParticleModifier
{
public:
    struct Parameter
    {
        int now;
        int count;
        xxVector2 offset;
        xxVector2 spread;
        float speed;
        float speedVariation;
        float tumble;
        float scale;
        float start;
        float stop;
        float life;
        float lifeVariation;
        float size;
        float sizeVariation;
        float grow;
        float fade;
        xxVector2 range;
        xxVector4 bound;

    public:
        void RuntimeAPI CalculateBound();
    };
//  static_assert(sizeof(Parameter) == 64);

    struct Header
    {
        int seed;
        Particle particles[];
    };
    static_assert(sizeof(Header) == 4);

public:
    void                    Update(void* target, float time, xxModifierData* data);

    static xxModifierPtr    Create();
};

#endif
