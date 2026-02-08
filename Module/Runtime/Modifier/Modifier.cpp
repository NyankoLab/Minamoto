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
#include "QuaternionModifier.h"
#include "ScaleModifier.h"
#include "TranslateModifier.h"
#include "InterpolatedFloatModifier.h"
#include "InterpolatedQuaternionModifier.h"
#include "InterpolatedScaleModifier.h"
#include "InterpolatedTranslateModifier.h"
#include "InterpolatedQuaternion16Modifier.h"
#include "BakedQuaternionModifier.h"
#include "BakedQuaternion16Modifier.h"
#if HAVE_PARTICLE
#include "Particle/SprayParticleModifier.h"
#endif
#include "Modifier.h"

extern "C" {
extern void* _ZTV13FloatModifier;
extern void* _ZTV14Float2Modifier;
extern void* _ZTV14Float3Modifier;
extern void* _ZTV14Float4Modifier;
extern void* _ZTV18QuaternionModifier;
extern void* _ZTV17TranslateModifier;
extern void* _ZTV13ScaleModifier;
extern void* _ZTV25InterpolatedFloatModifier;
extern void* _ZTV30InterpolatedQuaternionModifier;
extern void* _ZTV29InterpolatedTranslateModifier;
extern void* _ZTV25InterpolatedScaleModifier;
extern void* _ZTV32InterpolatedQuaternion16Modifier;
extern void* _ZTV23BakedQuaternionModifier;
extern void* _ZTV25BakedQuaternion16Modifier;
#if HAVE_PARTICLE
extern void* _ZTV21SprayParticleModifier;
#endif
}

static struct { std::string name; void** vtable; size_t header; size_t size; } const loaders[] =
{
    { "UNKNOWN",                    nullptr,                                    0,  1 },
    { "FLOAT",                      &_ZTV13FloatModifier,                       0,  sizeof(float) },
    { "FLOAT2",                     &_ZTV14Float2Modifier,                      0,  sizeof(xxVector2) },
    { "FLOAT3",                     &_ZTV14Float3Modifier,                      0,  sizeof(xxVector3) },
    { "FLOAT4",                     &_ZTV14Float4Modifier,                      0,  sizeof(xxVector4) },
    { "ARRAY",                      nullptr,                                    0,  sizeof(char) },
    { "STRING",                     nullptr,                                    0,  sizeof(char) },
                                {}, {}, {},
    { "QUATERNION",                 &_ZTV18QuaternionModifier,                  0,  sizeof(QuaternionModifier::Constant) },
    { "TRANSLATE",                  &_ZTV17TranslateModifier,                   0,  sizeof(TranslateModifier::Constant) },
    { "SCALE",                      &_ZTV13ScaleModifier,                       0,  sizeof(ScaleModifier::Constant) },
                                                        {}, {}, {},
    {},
    { "INTERPOLATED_FLOAT",         &_ZTV25InterpolatedFloatModifier,           0,  sizeof(InterpolatedFloatModifier::Key) },
            {}, {}, {}, {}, {}, {}, {}, {},
    { "INTERPOLATED_QUATERNION",    &_ZTV30InterpolatedQuaternionModifier,      0,  sizeof(InterpolatedQuaternionModifier::Key) },
    { "INTERPOLATED_TRANSLATE",     &_ZTV29InterpolatedTranslateModifier,       0,  sizeof(InterpolatedTranslateModifier::Key) },
    { "INTERPOLATED_SCALE",         &_ZTV25InterpolatedScaleModifier,           0,  sizeof(InterpolatedScaleModifier::Key) },
                                                        {}, {},
    { "INTERPOLATED_QUATERNION16",  &_ZTV32InterpolatedQuaternion16Modifier,    0,  sizeof(InterpolatedQuaternion16Modifier::Key) },
    {}, {}, {}, {}, {}, {}, {}, {}, {}, {},
    { "BAKED_QUATERNION",           &_ZTV23BakedQuaternionModifier,     sizeof(BakedQuaternionModifier::Baked),     sizeof(xxVector4) },
                                                {}, {}, {}, {},
    { "BAKED_QUATERNION16",         &_ZTV25BakedQuaternion16Modifier,   sizeof(BakedQuaternion16Modifier::Baked),   sizeof(v4hi) },
#if HAVE_PARTICLE
    { "SPRAY_PARTICLE",             &_ZTV21SprayParticleModifier,       sizeof(SprayParticleModifier::Parameter),   1 },
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
    if (type >= xxCountOf(loaders))
        type = UNKNOWN;
    if (type != UNKNOWN)
        const_cast<size_t&>(modifier.DataType) = type;

    if (loaders[type].vtable)
    {
        void* pointer = &modifier;
        void* vtable = loaders[type].vtable + 2;
        memcpy(pointer, &vtable, sizeof(void*));
    }
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
