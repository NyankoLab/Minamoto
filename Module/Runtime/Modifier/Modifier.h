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
        UNKNOWN                     = 0x00,
        FLOAT                       = 0x01,
        FLOAT2                      = 0x02,
        FLOAT3                      = 0x03,
        FLOAT4                      = 0x04,
        ARRAY                       = 0x05,
        STRING                      = 0x06,
        QUATERNION                  = 0x0A, // Node
        TRANSLATE                   = 0x0B, // Node
        SCALE                       = 0x0C, // Node
        INTERPOLATED_FLOAT          = 0x11,
        INTERPOLATED_FLOAT2         = 0x12,
        INTERPOLATED_FLOAT3         = 0x13,
        INTERPOLATED_FLOAT4         = 0x14,
        INTERPOLATED_QUATERNION     = 0x1A, // Node
        INTERPOLATED_TRANSLATE      = 0x1B, // Node
        INTERPOLATED_SCALE          = 0x1C, // Node
        INTERPOLATED_QUATERNION16   = 0x1F, // Node
        BAKED_QUATERNION            = 0x2A, // Node
        BAKED_TRANSLATE             = 0x2B, // Node
        BAKED_SCALE                 = 0x2C, // Node
        BAKED_QUATERNION16          = 0x2F, // Node
#if HAVE_PARTICLE
        SPRAY_PARTICLE              = 0x30, // Node
        SUPERSPRAY_PARTICLE         = 0x31, // Node
        BLIZZARD_PARTICLE           = 0x32, // Node
        PARTICLE_ARRAY              = 0x33, // Node
        PARTICLE_CLOUD              = 0x34, // Node
#endif
        MAX
    };

public:
    template<class T> void          AssignInterpolated(T& K);
    template<class T> bool          UpdateInterpolatedFactor(xxModifierData* data, float time, T*& A, T*& B, float& F);
    template<class T, class D> bool UpdateBakedFactor(xxModifierData* data, float time, D* baked, T*& A, T*& B, float& F);
    template<class T> T             Lerp(T const& A, T const &B, float F);

    static void                     Initialize();
    static void                     Shutdown();
    static void                     Loader(xxModifier& modifier, size_t type);
    static std::string const&       Name(xxModifier& modifier);
    static size_t                   Count(xxModifier& modifier);
    static size_t                   CalculateSize(size_t type, size_t count);
};
