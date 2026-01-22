//==============================================================================
// Minamoto : Import Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <map>
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include <Runtime/Tools/NodeTools.h>
#include "Import.h"

//#define STBI_NO_JPEG
//#define STBI_NO_PNG
#define STBI_NO_BMP
#define STBI_NO_PSD
#define STBI_NO_TGA
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_PIC
#define STBI_NO_PNM
#define STBI_NO_FAILURE_STRINGS
#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#if defined(__clang__)
#   pragma clang diagnostic ignored "-Wunused-function"
#endif
#include <stb/stb_image.h>

#define TAG "Import"

bool Import::EnableAxisUpYToZ = false;
bool Import::EnableMergeNode = false;
bool Import::EnableMergeTexture = false;
bool Import::EnableOptimizeMesh = false;
bool Import::EnableTextureFlipV = false;
//==============================================================================
void Import::Initialize()
{
}
//------------------------------------------------------------------------------
void Import::Shutdown()
{
}
//------------------------------------------------------------------------------
xxTexturePtr Import::CreateTexture(char const* img)
{
#if 1
    xxTexturePtr texture = xxTexture::Create();
    texture->Name = xxFile::GetName(img, true);
    texture->Path = xxFile::GetPath(img);
    xxTexture::Reader(texture);
#else
    int width = 1;
    int height = 1;
    stbi_uc* uc = stbi_load(img, &width, &height, nullptr, 4);

#if defined(xxWINDOWS)
    uint64_t format = "BGRA8888"_CC;
#else
    uint64_t format = "RGBA8888"_CC;
#endif
    xxTexturePtr texture = xxTexture::Create2D(format, width, height, 1);
    if (texture)
    {
        texture->Name = xxFile::GetName(img);
        if (uc)
        {
#if defined(xxWINDOWS)
            for (int i = 0; i < width * height * 4; i += 4)
            {
                std::swap(uc[i + 0], uc[i + 2]);
            }
#endif
            memcpy((*texture)(), uc, width * height * 4);
        }
    }

    stbi_image_free(uc);
#endif
    return texture;
}
//------------------------------------------------------------------------------
void Import::MergeNode(xxNodePtr const& target, xxNodePtr const& source, xxNodePtr const& root)
{
    std::vector<std::pair<xxNodePtr, xxNodePtr>> merge;
    std::vector<xxNodePtr> append;

    for (xxNodePtr const& right : *source)
    {
        bool found = false;
        for (xxNodePtr const& left : *target)
        {
            if (left->Name == right->Name)
            {
                merge.emplace_back(left, right);
                found = true;
                break;
            }
        }
        if (found)
            continue;

        append.push_back(right);
    }

    for (auto [left, right] : merge)
    {
        MergeNode(left, right, root);
        left->Modifiers = right->Modifiers;
    }

    for (auto node : append)
    {
        source->DetachChild(node);
        target->AttachChild(node);
    }

    Node::Traversal(target, [&](xxNodePtr const& node)
    {
        for (auto& data : node->Bones)
        {
            xxNodePtr from = data.bone.lock();
            xxNodePtr to;
            if (from)
            {
                to = NodeTools::GetObject(root, from->Name);
            }
            if (to == nullptr)
            {
                to = root;
                xxLog(TAG, "Bone %s is not found", from ? from->Name.c_str() : "(nullptr)");
            }
            data.bone = to;
        }
        return true;
    });
}
//------------------------------------------------------------------------------
void Import::MergeTexture(xxNodePtr const& node)
{
    std::map<std::string, xxTexturePtr> textures;

    Node::Traversal(node, [&](xxNodePtr const& node)
    {
        if (node->Material)
        {
            for (xxTexturePtr& texture : node->Material->Textures)
            {
                xxTexturePtr& ref = textures[texture->Name];
                if (ref)
                {
                    texture = ref;
                }
                else
                {
                    ref = texture;
                }
            }
        }
        return true;
    });
}
//------------------------------------------------------------------------------
std::string Import::CheckDuplicateName(xxNodePtr const& node, std::string const& name)
{
    std::string output = name;
    for (size_t i = node->GetChildCount(); i > 0; --i)
    {
        xxNodePtr const& child = node->GetChild(i - 1);
        if (child == nullptr)
            continue;
        if (child->Name.find(name) != 0)
            continue;
        output += '#';
        size_t sharp = child->Name.rfind('#');
        if (sharp == std::string::npos)
        {
            output += '0';
        }
        else
        {
            std::string name = child->Name.substr(sharp + 1);
            if (name.empty() == false && name[0] >= '0' && name[0] <= '9')
            {
                output += std::to_string(std::stoi(name) + 1);
            }
            else
            {
                output.pop_back();
            }
        }
        break;
    }
    return output;
}
//------------------------------------------------------------------------------
bool Import::CheckTextureAlpha(xxTexturePtr const& texture)
{
    if (texture == nullptr)
        return false;
    if (texture->Format != "RGBA8888"_CC)
        return false;

    int width = texture->Width;
    int height = texture->Height;
    int depth = texture->Depth;
    for (int z = 0; z < depth; ++z)
    {
        for (int y = 0; y < height; ++y)
        {
            uint8_t* pixel = static_cast<uint8_t*>((*texture)(0, y, z));
            for (int x = 0; x < width; ++x)
            {
                if (pixel[x * 4 + 3] != 0xFF)
                    return true;
            }
        }
    }

    return false;
}
//==============================================================================
