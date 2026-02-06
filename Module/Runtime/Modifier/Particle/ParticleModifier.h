//==============================================================================
// Minamoto : ParticleModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#if HAVE_PARTICLE

#include "Modifier/Modifier.h"

class RuntimeAPI ParticleModifier : public Modifier
{
public:
    struct Particle
    {
        xxVector3 point;
        xxVector3 velocity;
        float size;
        float age;
    };

public:
    void UpdateValues(float* values, size_t count, xxModifierData* data, float time);

public:
    static unsigned int Random(int& seed);
    static float RandomFloat(int& seed);
    static void SetParticleCount(Mesh* mesh, int count);
    static int SetParticleData(Mesh* mesh, Particle* particles, int count, float size);
};

#endif
