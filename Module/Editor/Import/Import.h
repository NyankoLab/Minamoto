//==============================================================================
// Minamoto : Import Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include <xxGraphicPlus/xxUtility.h>

struct Import
{
public:
    static void Initialize();
    static void Shutdown();
    static xxTexturePtr CreateTexture(char const* img);
    static void MergeNode(xxNodePtr const& target, xxNodePtr const& source, xxNodePtr const& root);
    static void MergeTexture(xxNodePtr const& node);
    static std::string CheckDuplicateName(xxNodePtr const& node, std::string const& name);
    static bool CheckTextureAlpha(xxTexturePtr const& texture);
public:
    static bool EnableAxisUpYToZ;
    static bool EnableMergeNode;
    static bool EnableMergeTexture;
    static bool EnableOptimizeMesh;
    static bool EnableTextureFlipV;
public:
    typedef std::function<void(void*, void*, xxNodePtr&&, std::function<void(xxNodePtr const&)>)> ImportCallback;
};
