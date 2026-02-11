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
        float radian;
        float size;
        float age;

        xxVector3 velocity;
        float spin;
        float fade;
        float grow;
    };

public:
    void UpdateValues(float* values, size_t count, float time, xxModifierData* data);

public:
    static int Random(int& seed);
    static float RandomFloat(int& seed);
    static xxVector2 RandomFloat2(int& seed);
    static xxVector3 RandomFloat3(int& seed);
    static void SetParticleCount(Mesh* mesh, int count);
    static int SetParticleData(Mesh* mesh, Particle* particles, int count, float size);
};

#endif
