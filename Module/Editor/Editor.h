//==============================================================================
// Minamoto : Editor Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <xxGraphic/xxGraphic.h>
#include <Runtime/Runtime.h>

#ifdef _WIN32
#undef IMGUI_API
#define IMGUI_API __declspec(dllimport)
#endif

#include <Interface.h>
#include "ImGuiHelper.h"

#ifdef _WIN32
#undef IMGUI_API
#define IMGUI_API
#endif

#include <IconFontCppHeaders/IconsFontAwesome4.h>

#define MODULE_NAME "Editor"
