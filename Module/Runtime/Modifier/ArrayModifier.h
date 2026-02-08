//==============================================================================
// Minamoto : ArrayModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI ArrayModifier : public Modifier
{
public:
    void                    Set(std::span<char> span);
    std::span<char>         Get() const;

public:
    static xxModifierPtr    Create(std::span<char> span = {});
};
