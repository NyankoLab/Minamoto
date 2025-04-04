//==============================================================================
// Minamoto : Renderer Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <xxGraphic/xxGraphic.h>

class Renderer
{
public:
    static bool Create(void* view, int width, int height, char const* shortName = nullptr);
    static void Reset(void* view, int width, int height);
    static void Shutdown();

    static uint64_t Begin();
    static void End();
    static bool Present();

    static char const* GetCurrentFullName();
    static char const* GetGraphicFullName(int index);
    static char const* GetGraphicShortName(int index);

public:
    static uint64_t g_instance;
    static uint64_t g_device;
    static uint64_t g_renderPass;
    static uint64_t g_swapchain;

    static uint64_t g_currentCommandBuffer;
    static uint64_t g_currentCommandEncoder;
    static uint64_t g_currentCommandFramebuffer;

    static void*    g_view;
    static int      g_width;
    static int      g_height;
    static float    g_scale;
    static float    g_clearColor[4];
    static float    g_clearDepth;
    static char     g_clearStencil;
};
