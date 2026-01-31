//==============================================================================
// Minamoto : RainParticleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Mesh.h"
#include "Graphic/Node.h"
#include "RainParticleModifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  RainParticleModifier
//==============================================================================
void RainParticleModifier::Update(void* target, xxModifierData* data, float time)
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

    int particleBorn = 0;
    int particleBirth = 0;
    int particleCount = parameter->count;
    float particleLife = parameter->life;
    float particleVariation = parameter->variation;

    if (parameter->now < parameter->count)
    {
        float rate = particleCount / particleLife;
        float during = time - parameter->start;
        float birthRate = parameter->birth * rate;
        particleBirth = std::min<int>(truncf(during * birthRate) - truncf((during - delta) * birthRate), during * rate);
    }

    size_t tempSize = sizeof(Header) + sizeof(Particle) * particleCount;
    if (data->temp.size() < tempSize)
        data->temp.resize(tempSize);

    Header* header = (Header*)data->temp.data();
    if (header->bound.w == 0.0f)
        CalculateBound(data);
    if (header->seed == 0)
        header->seed = (int)(size_t)mesh;

    Particle* particles = (Particle*)&header[1];
    for (int i = 0; i < particleCount; ++i)
    {
        Particle& particle = particles[i];
        if (particle.age >= particleLife)
            particle.age = 0.0f;
        if (particle.age == 0.0f)
        {
            if (particleBorn >= particleBirth)
                continue;
            particleBorn++;
            particle.point.x = (RandomFloat(header->seed) - 0.5f) * parameter->width;
            particle.point.y = (RandomFloat(header->seed) - 0.5f) * parameter->height;
            particle.point.z = 0.0f;
            particle.velocity.x = 0.0f;
            particle.velocity.y = 0.0f;
            particle.velocity.z = -parameter->speed;
            if (particleVariation != 0.0f)
            {
                particle.velocity.x += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
                particle.velocity.y += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
                particle.velocity.z += (RandomFloat(header->seed) * 2.0f - 1.0f) * parameter->variation;
            }
            particle.age = FLT_MIN;
            continue;
        }
        particle.age += delta;
        particle.point += particle.velocity * delta;
    }

    const_cast<xxVector4&>(mesh->Bound) = header->bound;
    parameter->now = SetParticleData(mesh, particles, particleCount, parameter->size);
}
//------------------------------------------------------------------------------
void RainParticleModifier::CalculateBound(xxModifierData* data)
{
    Parameter* parameter = (Parameter*)Data.data();
    Header* header = (Header*)data->temp.data();
    xxVector4 p0;
    xxVector4 p1;
    p0.x = parameter->width * -0.5f;
    p1.x = parameter->width *  0.5f;
    p0.y = parameter->height * -0.5f;
    p1.y = parameter->height *  0.5f;
    p0.z = parameter->speed > 0.0f ? 0.0f : -parameter->speed * parameter->life;
    p1.z = parameter->speed < 0.0f ? 0.0f : -parameter->speed * parameter->life;
    p0.w = parameter->size;
    p1.w = parameter->size;
    header->bound = p0.BoundMerge(p1);
}
//------------------------------------------------------------------------------
xxModifierPtr RainParticleModifier::Create()
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Parameter));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, RAIN_PARTICLE);
    return modifier;
}
//==============================================================================
