//==============================================================================
// Minamoto : NodeTools Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"

struct RuntimeAPI NodeTools
{
    static size_t constexpr TEST_CHECK_FLAG = size_t(1) << (sizeof(size_t) * 8 - 1);
#if HAVE_MINIGUI
    static MiniGUI::WindowPtr const& GetRoot(MiniGUI::WindowPtr const& window);
#endif
    static xxNodePtr const& GetRoot(xxNodePtr const& node);
    static xxNodePtr const& GetObject(xxNodePtr const& node, std::string const& name);
    static xxNodePtr const& Intersect(xxNodePtr const& node, xxVector3 const& position, xxVector3 const& direction);
    static void ConvertQuadTree(xxNodePtr const& node);
    static void RemoveEmptyNode(xxNodePtr const& node);
    static void ResetBoneMatrix(xxNodePtr const& node);
    static void UpdateNodeFlags(xxNodePtr const& node);
};
