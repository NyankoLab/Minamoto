//==============================================================================
// Minamoto : Camera Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxCamera.h>

struct RuntimeAPI Camera : public xxCamera
{
public:
    float       LambertRound = 0.5f;

public:
    static void Initialize();
    static void Shutdown();
};
