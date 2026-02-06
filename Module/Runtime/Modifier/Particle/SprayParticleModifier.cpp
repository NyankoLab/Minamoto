//==============================================================================
// Minamoto : SprayParticleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Mesh.h"
#include "Graphic/Node.h"
#include "SprayParticleModifier.h"
#include "Modifier/Modifier.inl"

#if HAVE_PARTICLE

//==============================================================================
//  SprayParticleModifier
//==============================================================================
void SprayParticleModifier::Update(void* target, xxModifierData* data, float time)
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

    UpdateValues(&parameter->size, xxCountOf(header->modifiers), header->modifiers, time);

    int particleBorn = 0;
    int particleBirth = 0;

    if (parameter->now < parameter->count)
    {
        float rate = parameter->count / parameter->life;
        float during = time - parameter->start;
        float birthRate = parameter->birth * rate;
        particleBirth = std::min<int>(truncf(during * birthRate) - truncf((during - delta) * birthRate), during * rate);
    }

    Particle* particles = header->particles;
    for (int i = 0; i < parameter->count; ++i)
    {
        Particle& particle = particles[i];
        if (particle.age <= 0.0f)
        {
            if (particleBorn >= particleBirth)
                continue;
            particleBorn++;
            particle.point.x = (RandomFloat(header->seed) - 0.5f) * parameter->range.x;
            particle.point.y = (RandomFloat(header->seed) - 0.5f) * parameter->range.y;
            particle.point.z = 0.0f;
            particle.velocity.x = 0.0f;
            particle.velocity.y = 0.0f;
            particle.velocity.z = -parameter->speed;
            if (parameter->variation != 0.0f)
            {
                particle.velocity.x += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
                particle.velocity.y += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
                particle.velocity.z += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
            }
            particle.size = parameter->size;
            particle.age = parameter->life;
            continue;
        }
        particle.age -= delta;
        particle.point += particle.velocity * delta;
    }

    const_cast<xxVector4&>(mesh->Bound) = parameter->bound;
    parameter->now = SetParticleData(mesh, particles, parameter->count, parameter->size);
}
//------------------------------------------------------------------------------
void SprayParticleModifier::Parameter::CalculateBound()
{
    xxVector4 p0;
    xxVector4 p1;
    float velocity = speed + copysignf(variation, speed);
    p0.x = range.x * -0.5f;
    p1.x = range.x *  0.5f;
    p0.y = range.y * -0.5f;
    p1.y = range.y *  0.5f;
    p0.z = speed > 0.0f ? 0.0f : -velocity * life;
    p1.z = speed < 0.0f ? 0.0f : -velocity * life;
    p0.radius = size;
    p1.radius = size;
    bound = p0.BoundMerge(p1);
}
//------------------------------------------------------------------------------
xxModifierPtr SprayParticleModifier::Create()
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Parameter));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, SPRAY_PARTICLE);
    return modifier;
}
//==============================================================================

#endif
