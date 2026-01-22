//==============================================================================
// Minamoto : Camera Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxCamera.h>

struct RuntimeAPI Camera : public xxCamera
{
public:
    float       LambertStep = 0.5f;

public:
    static void Initialize();
    static void Shutdown();
};

#if defined(xxWINDOWS)
#if defined(_LIBCPP_VERSION)
#pragma comment(linker, "/alternatename:__imp_?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@__1@std@@XZ=__imp_?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@UCamera@@@__1@std@@XZEA=?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@VxxCamera@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@__1@std@@XZ=?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@__1@std@@XZ")
#else
#pragma comment(linker, "/alternatename:__imp_?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@std@@XZ=__imp_?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@std@@XZ")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@UCamera@@@std@@XZEA=?BinaryCreate@xxCamera@@2P6A?AV?$shared_ptr@VxxCamera@@@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxCamera@@SA?AV?$shared_ptr@UCamera@@@std@@XZ=?Create@xxCamera@@SA?AV?$shared_ptr@VxxCamera@@@std@@XZ")
#endif
#endif
