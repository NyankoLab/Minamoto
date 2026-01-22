//==============================================================================
// Minamoto : TextureTools Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Node.h>
#include <Runtime/Graphic/Texture.h>
#include "TextureTools.h"

#define TAG "TextureTools"

#define STB_DXT_IMPLEMENTATION
#ifdef __clang__
#pragma clang diagnostic ignored "-Wcomma"
#endif
#include <stb/stb_dxt.h>
static void stb__CompressColorBlockEx(unsigned char* dest, unsigned char* block, int mode)
{
    for (int i = 0; i < 16; ++i)
    {
        dest[i * 4 + 3] = 0xFF;
    }
    stb__CompressColorBlock(dest, block, mode);
}
static void stb__ReduceAlphaBlock(unsigned char* dest, unsigned char* src, int stride)
{
    memset(dest, 0, 8);
    for (int i = 0; i < 16; ++i)
    {
        dest[i / 2] |= (src[i * stride] & 0xF0) >> (((i + 1) % 2) * 4);
    }
}
static void stb__CompressAlphaBlockEx(unsigned char* dest, unsigned char* src, int stride)
{
    stb__CompressAlphaBlock(dest, src, stride);
    if (dest[0] == dest[1])
    {
        if (dest[1] == 0xFF)
            memset(dest + 2, 0xFF, 6);
        else if (dest[2] == 0x49)
            memset(dest + 2, 0, 6);
    }
}

//==============================================================================
void TextureTools::CompressTexture(xxTexturePtr const& texture, uint64_t format, std::string const& root, std::string const& subfolder)
{
    if (texture->Format != "RGBA8888"_CC)
        return;
    switch (format)
    {
    case "BC1"_cc:  case "DXT1"_cc:
    case "BC2"_cc:  case "DXT3"_cc:
    case "BC3"_cc:  case "DXT5"_cc:
    case "BC4U"_cc: case "ATI1"_cc:
    case "BC5U"_cc: case "ATI2"_cc:
        break;
    default:
        return;
    }
    xxTexturePtr uncompressed = xxTexture::Create();
    uncompressed->Name = texture->Name;
    uncompressed->Path = texture->Path;
    xxTexture::Reader(uncompressed);
    if ((*uncompressed)() == nullptr)
        return;
    xxTexturePtr compressed = xxTexture::Create(format, uncompressed->Width, uncompressed->Height, uncompressed->Depth, uncompressed->Mipmap, uncompressed->Array);
    for (int array = 0; array < uncompressed->Array; ++array)
    {
        for (int mipmap = 0; mipmap < uncompressed->Mipmap; ++mipmap)
        {
            int levelWidth = (uncompressed->Width >> mipmap);
            int levelHeight = (uncompressed->Height >> mipmap);
            int levelDepth = (uncompressed->Depth >> mipmap);
            if (levelWidth == 0)
                levelWidth = 1;
            if (levelHeight == 0)
                levelHeight = 1;
            if (levelDepth == 0)
                levelDepth = 1;

            void* line = (*compressed)(0, 0, 0, mipmap, array);
            for (int depth = 0; depth < levelDepth; ++depth)
            {
                for (int height = 0; height < levelHeight; height += 4)
                {
                    for (int width = 0; width < levelWidth; width += 4)
                    {
                        unsigned char source[4 * 4 * 4];
                        for (int y = 0; y < 4; ++y)
                        {
                            for (int x = 0; x < 4; ++x)
                            {
                                int offsetX = std::min(width + x, levelWidth - 1);
                                int offsetY = std::min(height + y, levelHeight - 1);
                                memcpy(source + y * 4 * 4 + x * 4, (*uncompressed)(offsetX, offsetY, depth, mipmap, array), 4);
                            }
                        }
                        switch (format)
                        {
                        case "BC1"_cc:
                        case "DXT1"_cc:
                            stb__CompressColorBlockEx((unsigned char*)line, source, STB_DXT_HIGHQUAL);
                            line = (char*)line + 8;
                            break;
                        case "BC2"_cc:
                        case "DXT3"_cc:
                            stb__ReduceAlphaBlock((unsigned char*)line, source + 3, 4);
                            stb__CompressColorBlockEx((unsigned char*)line + 8, source, STB_DXT_HIGHQUAL);
                            line = (char*)line + 16;
                            break;
                        case "BC3"_cc:
                        case "DXT5"_cc:
                            stb__CompressAlphaBlockEx((unsigned char*)line, source + 3, 4);
                            stb__CompressColorBlockEx((unsigned char*)line + 8, source, STB_DXT_HIGHQUAL);
                            line = (char*)line + 16;
                            break;
                        case "BC4U"_cc:
                        case "ATI1"_cc:
                            stb__CompressAlphaBlockEx((unsigned char*)line, source, 4);
                            line = (char*)line + 8;
                            break;
                        case "BC5U"_cc:
                        case "ATI2"_cc:
                            stb__CompressAlphaBlockEx((unsigned char*)line, source, 4);
                            stb__CompressAlphaBlockEx((unsigned char*)line + 8, source + 1, 4);
                            line = (char*)line + 16;
                            break;
                        }
                    }
                }
            }
        }
    }
    std::string ext = ".unknown";
    switch (format)
    {
    case "BC1"_cc:  case "DXT1"_cc: ext = ".bc1";   break;
    case "BC2"_cc:  case "DXT3"_cc: ext = ".bc2";   break;
    case "BC3"_cc:  case "DXT5"_cc: ext = ".bc3";   break;
    case "BC4U"_cc: case "ATI1"_cc: ext = ".bc4u";  break;
    case "BC5U"_cc: case "ATI2"_cc: ext = ".bc5u";  break;
    }
    Texture::DDSWriter(compressed, root + subfolder + uncompressed->Name + ext);
}
//------------------------------------------------------------------------------
void TextureTools::MipmapTexture(xxTexturePtr const& texture)
{
    if (texture == nullptr)
        return;
    if (texture->Depth > 1 || texture->Mipmap > 1 || texture->Array > 1)
        return;
    if (texture->Format != "RGBA8888"_CC && texture->Format != "BGRA8888"_CC)
        return;
    if ((*texture)() == nullptr)
        return;

    size_t size = xxTexture::Calculate(texture->Format, texture->Width, texture->Height, texture->Depth);
    if (size == 0)
        return;

    int max = std::max(std::max(texture->Width, texture->Height), texture->Depth);
    int mipmap = (int)log2(max) + 1;

    void* image = xxAlloc(unsigned char, size);
    if (image == nullptr)
        return;

    float begin = xxGetCurrentTime();

    memcpy(image, (*texture)(), size);
    texture->Initialize(texture->Format, texture->Width, texture->Height, texture->Depth, mipmap, texture->Array);
    memcpy((*texture)(), image, size);
    xxFree(image);

    for (int array = 0; array < texture->Array; ++array)
    {
        for (int mipmap = 1; mipmap < texture->Mipmap; ++mipmap)
        {
            int levelWidth = (texture->Width >> mipmap);
            int levelHeight = (texture->Height >> mipmap);
            int levelDepth = (texture->Depth >> mipmap);
            if (levelWidth == 0)
                levelWidth = 1;
            if (levelHeight == 0)
                levelHeight = 1;
            if (levelDepth == 0)
                levelDepth = 1;

            for (int depth = 0; depth < levelDepth; ++depth)
            {
                for (int height = 0; height < levelHeight; ++height)
                {
                    for (int width = 0; width < levelWidth; ++width)
                    {
                        int r = 0;
                        int g = 0;
                        int b = 0;
                        int a = 0;
                        int count = 0;

                        int width2 = width * 2;
                        int height2 = height * 2;
                        int depth2 = depth * 2;
                        for (int z = 0; z < 2; ++z)
                        {
                            for (int y = 0; y < 2; ++y)
                            {
                                for (int x = 0; x < 2; ++x)
                                {
                                    unsigned char* right = (unsigned char*)(*texture)(width2 + x, height2 + y, depth2 + z, mipmap - 1, array);
                                    if (right)
                                    {
                                        r += right[0];
                                        g += right[1];
                                        b += right[2];
                                        a += right[3];
                                        count++;
                                    }
                                }
                            }
                        }

                        unsigned char* left = (unsigned char*)(*texture)(width, height, depth, mipmap, array);
                        if (left && count)
                        {
                            left[0] = r / count;
                            left[1] = g / count;
                            left[2] = b / count;
                            left[3] = a / count;
                        }
                    }
                }
            }
        }
    }

    texture->Invalidate();

    xxLog(TAG, "MipmapTexture : %s (%.0fus)", texture->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);
}
//------------------------------------------------------------------------------
void TextureTools::MipmapTextures(xxNodePtr const& node)
{
    Node::Traversal(node, [](xxNodePtr const& node)
    {
        if (node->Material)
        {
            for (xxTexturePtr const& texture : node->Material->Textures)
            {
                MipmapTexture(texture);
            }
        }
        return true;
    });
}
//==============================================================================
