//==============================================================================
// Minamoto : NodeTools Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"

struct RuntimeAPI NodeTools
{
    static constexpr size_t TEST_CHECK_FLAG = size_t(1) << (sizeof(size_t) * 8 - 1);
#if HAVE_MINIGUI
    static MiniGUI::WindowPtr const& GetRoot(MiniGUI::WindowPtr const& window);
#endif
    static xxNodePtr const& GetRoot(xxNodePtr const& node);
    static xxNodePtr const& GetObject(xxNodePtr const& node, std::string const& name);
    static void ConvertQuadTree(xxNodePtr const& node);
    static void RemoveEmptyNode(xxNodePtr const& node);
    static void UpdateNodeFlags(xxNodePtr const& node);
};
