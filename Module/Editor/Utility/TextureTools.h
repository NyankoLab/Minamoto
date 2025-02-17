//==============================================================================
// Minamoto : TextureTools Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

struct TextureTools
{
    static void CompressTexture(xxTexturePtr const& texture, uint64_t format, std::string const& root, std::string const& subfolder);
    static void MipmapTexture(xxTexturePtr const& texture);
    static void MipmapTextures(xxNodePtr const& node);
};
