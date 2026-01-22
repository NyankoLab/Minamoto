//==============================================================================
// Minamoto : CSV Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <functional>
#include <string>
#include <vector>

struct RuntimeAPI CSV
{
    static bool Load(char const* name, std::function<void(std::vector<std::string_view> const&)> deserialize, std::string_view separate = ",");
    static bool Save(char const* name, std::function<void(std::vector<std::string_view>&)> serialize, std::string_view separate = ",");
};
