//==============================================================================
// Minamoto : ParticleTools Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

struct ParticleTools
{
    static xxNodePtr CreateParticle(uint64_t type);
    static void ConvertParticle(xxModifier& modifier, uint64_t type);
};
