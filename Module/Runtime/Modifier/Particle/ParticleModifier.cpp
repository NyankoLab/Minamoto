//==============================================================================
// Minamoto : ParticleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Mesh.h"
#include "Graphic/Node.h"
#include "ParticleModifier.h"

//==============================================================================
//  ParticleModifier
//==============================================================================
unsigned int ParticleModifier::Random(int& seed)
{
    return seed = (seed * 214013) + 2531011;
}
//------------------------------------------------------------------------------
float ParticleModifier::RandomFloat(int& seed)
{
    return Random(seed) / (float)UINT32_MAX;
}
//------------------------------------------------------------------------------
void ParticleModifier::SetParticleCount(Mesh* mesh, int count)
{
    mesh->SetIndexCount(count * 6);
    mesh->SetVertexCount(count * 4);

    auto colors = mesh->GetColor();
    for (auto& color : colors)
    {
        color = 0xFFFFFFFF;
    }

    auto textures = mesh->GetTexture();
    for (int i = 0; auto& texture : textures)
    {
        switch (i++ % 4)
        {
        case 0: texture = { 0.0f, 0.0f };   break;
        case 1: texture = { 0.0f, 1.0f };   break;
        case 2: texture = { 1.0f, 1.0f };   break;
        case 3: texture = { 1.0f, 0.0f };   break;
        }
    }

    auto indices = (uint16_t*)mesh->Index;
    for (int i = 0; i < count; ++i)
    {
        indices[i * 6 + 0] = (i * 4 + 0);
        indices[i * 6 + 1] = (i * 4 + 1);
        indices[i * 6 + 2] = (i * 4 + 2);
        indices[i * 6 + 3] = (i * 4 + 0);
        indices[i * 6 + 4] = (i * 4 + 2);
        indices[i * 6 + 5] = (i * 4 + 3);
    }
}
//------------------------------------------------------------------------------
int ParticleModifier::SetParticleData(Mesh* mesh, Particle* particles, int count, float size)
{
    if (mesh->VertexCount < count * 4)
        SetParticleCount(mesh, count);
    mesh->SetVertexCount(count * 4);

    int now = 0;

    auto positions = mesh->GetPosition();
    for (int i = 0; i < count; ++i)
    {
        Particle& particle = particles[i];
        if (particle.age == 0.0f)
            continue;
        auto& point = particle.point;
        (*positions++) = point + xxVector3{ -size, -size, 0.0f };
        (*positions++) = point + xxVector3{ -size,  size, 0.0f };
        (*positions++) = point + xxVector3{  size,  size, 0.0f };
        (*positions++) = point + xxVector3{  size, -size, 0.0f };
        now++;
    }

    mesh->ActiveCount[Mesh::INDEX] = now * 6;
    mesh->ActiveCount[Mesh::VERTEX] = now * 4;

    return now;
}
//==============================================================================
