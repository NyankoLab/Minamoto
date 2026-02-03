//==============================================================================
// Minamoto : Modifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "FloatModifier.h"
#include "Float2Modifier.h"
#include "Float3Modifier.h"
#include "Float4Modifier.h"
#include "StringModifier.h"
#include "ConstantQuaternionModifier.h"
#include "ConstantScaleModifier.h"
#include "ConstantTranslateModifier.h"
#include "QuaternionModifier.h"
#include "ScaleModifier.h"
#include "TranslateModifier.h"
#include "BakedQuaternionModifier.h"
#include "Quaternion16Modifier.h"
#include "BakedQuaternion16Modifier.h"
#if HAVE_PARTICLE
#include "Particle/SnowParticleModifier.h"
#endif
#include "Modifier.h"

#define UPDATE(class)   reinterpret_cast<void(xxModifier::*)(void*, xxModifierData*, float)>(&class::Update)

static struct { std::string name; xxModifier::UpdateDeclaration function; size_t header; size_t size; } const loaders[] =
{
    { "UNKNOWN",              &xxModifier::UpdateDummy,           0,                                        1 },
    { "FLOAT",                &xxModifier::UpdateDummy,           0,                                        sizeof(float) },
    { "FLOAT2",               &xxModifier::UpdateDummy,           0,                                        sizeof(xxVector2) },
    { "FLOAT3",               &xxModifier::UpdateDummy,           0,                                        sizeof(xxVector3) },
    { "FLOAT4",               &xxModifier::UpdateDummy,           0,                                        sizeof(xxVector4) },
    { "ARRAY",                &xxModifier::UpdateDummy,           0,                                        sizeof(char) },
    { "STRING",               &xxModifier::UpdateDummy,           0,                                        sizeof(char) },
                                {}, {}, {}, {}, {}, {}, {}, {}, {},
    { "CONSTANT_QUATERNION",  UPDATE(ConstantQuaternionModifier), 0,                                        sizeof(ConstantQuaternionModifier::Constant) },
    { "CONSTANT_TRANSLATE",   UPDATE(ConstantTranslateModifier),  0,                                        sizeof(ConstantTranslateModifier::Constant) },
    { "CONSTANT_SCALE",       UPDATE(ConstantScaleModifier),      0,                                        sizeof(ConstantScaleModifier::Constant) },
                {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    { "QUATERNION",           UPDATE(QuaternionModifier),         0,                                        sizeof(QuaternionModifier::Key) },
    { "TRANSLATE",            UPDATE(TranslateModifier),          0,                                        sizeof(TranslateModifier::Key) },
    { "SCALE",                UPDATE(ScaleModifier),              0,                                        sizeof(ScaleModifier::Key) },
                {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    { "QUATERNION16",         UPDATE(Quaternion16Modifier),       0,                                        sizeof(Quaternion16Modifier::Key) },
    { "BAKED_QUATERNION",     UPDATE(BakedQuaternionModifier),    sizeof(BakedQuaternionModifier::Baked),   sizeof(xxVector4) },
        {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    { "BAKED_QUATERNION16",   UPDATE(BakedQuaternion16Modifier),  sizeof(BakedQuaternion16Modifier::Baked), sizeof(v4hi) },
#if HAVE_PARTICLE
    { "SNOW_PARTICLE",        UPDATE(SnowParticleModifier),       sizeof(SnowParticleModifier::Parameter),  1 },
#endif
};
static_assert(xxCountOf(loaders) == Modifier::MAX);
//==============================================================================
void Modifier::Initialize()
{
    xxModifier::Loader = Modifier::Loader;
}
//------------------------------------------------------------------------------
void Modifier::Shutdown()
{
    xxModifier::Loader = [](xxModifier&, size_t) {};
}
//------------------------------------------------------------------------------
void Modifier::Loader(xxModifier& modifier, size_t type)
{
    if (type >= xxCountOf(loaders) || loaders[type].function == nullptr)
        type = UNKNOWN;
    const_cast<xxModifier::UpdateDeclaration&>(modifier.UpdateFunction) = loaders[type].function;
    if (type != UNKNOWN)
        const_cast<size_t&>(modifier.DataType) = type;
}
//------------------------------------------------------------------------------
std::string const& Modifier::Name(xxModifier& modifier)
{
    size_t type = modifier.DataType;
    if (type >= xxCountOf(loaders))
        type = UNKNOWN;

    return loaders[type].name;
}
//------------------------------------------------------------------------------
size_t Modifier::Count(xxModifier& modifier)
{
    size_t type = modifier.DataType;
    if (type >= xxCountOf(loaders))
        type = UNKNOWN;

    size_t size = modifier.Data.size();
    if (size < loaders[type].header)
        return 0;
    return (size - loaders[type].header) / loaders[type].size;
}
//------------------------------------------------------------------------------
size_t Modifier::CalculateSize(size_t type, size_t count)
{
    if (type >= xxCountOf(loaders))
        type = UNKNOWN;

    return loaders[type].header + loaders[type].size * count;
}
//==============================================================================
