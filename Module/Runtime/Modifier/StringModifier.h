//==============================================================================
// Minamoto : StringModifier Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Modifier.h"

class RuntimeAPI StringModifier : public Modifier
{
public:
    void                    Set(std::string_view const& string);
    std::string_view        Get() const;

    static xxModifierPtr    Create(std::string_view const& string = "");
};
