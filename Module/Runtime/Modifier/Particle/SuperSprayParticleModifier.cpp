//==============================================================================
// Minamoto : SuperSprayParticleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Mesh.h"
#include "Graphic/Node.h"
#include "SuperSprayParticleModifier.h"
#include "Modifier/Modifier.inl"

#if HAVE_PARTICLE

//==============================================================================
//  SuperSprayParticleModifier
//==============================================================================
void SuperSprayParticleModifier::Update(void* target, float time, xxModifierData* data)
{
    if (data->time == time)
        return;
    float delta = time - data->time;
    data->time = time;

    if (data->start == 0.0f)
        data->start = time;
    time -= data->start;

    Node* node = (Node*)target;
    Mesh* mesh = (Mesh*)node->Mesh.get();
    if (mesh == nullptr)
        return;

    Parameter* parameter = (Parameter*)Data.data();

    size_t tempSize = sizeof(Header) + sizeof(Particle) * parameter->count;
    if (data->temp.size() < tempSize)
        data->temp.resize(tempSize);

    Header* header = (Header*)data->temp.data();
    if (header->seed == 0)
        header->seed = (int)(size_t)mesh;

    int particleBorn = 0;
    int particleBirth = 0;

    if (parameter->now < parameter->count)
    {
        float rate = parameter->count / parameter->life;
        float during = time - parameter->start;
        float birthRate = rate;
        particleBirth = std::min<int>(truncf(during * birthRate) - truncf((during - delta) * birthRate), during * rate);
    }

    Particle* particles = header->particles;
    for (Particle& particle : std::span(particles, parameter->count))
    {
        if (particle.age <= 0.0f)
        {
            if (particleBorn >= particleBirth)
                continue;
            particleBorn++;
            particle.point.xy = (RandomFloat2(header->seed) - 0.5f) * parameter->range;
            particle.point.z = 0.0f;
            xxVector2 theta = parameter->offset + RandomFloat2(header->seed) * parameter->spread;
            particle.velocity.x = sinf(theta.x) * sinf(theta.y);
            particle.velocity.y = sinf(theta.x) * cosf(theta.y);
            particle.velocity.z = cosf(theta.x);
            particle.velocity = particle.velocity * parameter->speed;
            if (parameter->speedVariation != 0.0f)
            {
                particle.velocity += (RandomFloat3(header->seed) * 2.0f - 1.0f) * parameter->speedVariation;
            }
            particle.size = parameter->size;
            if (parameter->sizeVariation != 0.0f)
            {
                particle.size += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->sizeVariation;
            }
            particle.age = parameter->life;
            if (parameter->lifeVariation != 0.0f)
            {
                particle.age += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->lifeVariation;
            }
            continue;
        }
        particle.age -= delta;
        particle.point += particle.velocity * delta;
    }

    const_cast<xxVector4&>(mesh->Bound) = parameter->bound;
    parameter->now = SetParticleData(mesh, particles, parameter->count, parameter->size);
}
//------------------------------------------------------------------------------
void SuperSprayParticleModifier::Parameter::CalculateBound()
{
    float velocity = speed + copysignf(speedVariation, speed);
    xxVector4 p0;
    xxVector4 p1;
    p0.x = range.x * -0.5f + velocity * -life;
    p1.x = range.x *  0.5f + velocity *  life;
    p0.y = range.y * -0.5f + velocity * -life;
    p1.y = range.y *  0.5f + velocity *  life;
    p0.z = velocity * -life;
    p1.z = velocity *  life;
    p0.radius = size;
    p1.radius = size;
    bound = p0.BoundMerge(p1);
}
//------------------------------------------------------------------------------
xxModifierPtr SuperSprayParticleModifier::Create()
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Parameter));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, SUPERSPRAY_PARTICLE);
    return modifier;
}
//==============================================================================

#endif
