//==============================================================================
// Minamoto : Dear ImGui Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include <stdint.h>
#include <imgui/imgui.h>

class DearImGui
{
public:
    static void Create(void* view, float scale, float font);
    static void Shutdown();

    static void Suspend();
    static void Resume();

    static void NewFrame(void* view);
    static bool Update(bool demo);
    static void* PostUpdate(void* view, bool render);

    static void Render(uint64_t commandEncoder);

    static bool PowerSaving();

    static void HandleEventAndroid(int type, float x, float y);

protected:
    static char const*  g_graphicShortName;
    static bool         g_recreateWindow;
    static bool         g_powerSaving;
    static bool         g_demoWindow;
};
