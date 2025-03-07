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

#if defined(xxWINDOWS)
#pragma comment(linker, "/alternatename:__imp_?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@__1@std@@XZ=__imp_?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@UCamera@@@__1@std@@XZEA=?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@VxxCamera@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@__1@std@@XZ=?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@__1@std@@XZ")
#endif
