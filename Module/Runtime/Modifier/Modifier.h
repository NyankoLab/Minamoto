//==============================================================================
// Minamoto : Modifier Inline
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxModifier.h>

class RuntimeAPI Modifier : public xxModifier
{
public:
    enum Type
    {
        UNKNOWN             = 0x00,
        FLOAT               = 0x01,
        FLOAT2              = 0x02,
        FLOAT3              = 0x03,
        FLOAT4              = 0x04,
        ARRAY               = 0x05,
        STRING              = 0x06,
        CONSTANT_QUATERNION = 0x10,
        CONSTANT_TRANSLATE  = 0x11,
        CONSTANT_SCALE      = 0x12,
        QUATERNION          = 0x20,
        TRANSLATE           = 0x21,
        SCALE               = 0x22,
        QUATERNION16        = 0x2F,
        BAKED_QUATERNION    = 0x30,
        BAKED_TRANSLATE     = 0x31,
        BAKED_SCALE         = 0x32,
        BAKED_QUATERNION16  = 0x3F,
#if HAVE_PARTICLE
        SNOW_PARTICLE       = 0x40,
#endif
        MAX
    };

public:
    template<class T> bool          UpdateKeyFactor(xxModifierData* data, float time, T*& A, T*& B, float& F);
    template<class T, class D> bool UpdateBakedFactor(xxModifierData* data, float time, D* baked, T*& A, T*& B, float& F);
    template<class T> T             Lerp(T const& A, T const &B, float F);

    static void                     Initialize();
    static void                     Shutdown();
    static void                     Loader(xxModifier& modifier, size_t type);
    static std::string const&       Name(xxModifier& modifier);
    static size_t                   Count(xxModifier& modifier);
    static size_t                   CalculateSize(size_t type, size_t count);
};
