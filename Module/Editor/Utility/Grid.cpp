//==============================================================================
// Minamoto : Grid Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include "Grid.h"

//------------------------------------------------------------------------------
xxNodePtr Grid::Create(xxVector3 const& translate, xxVector2 const& size)
{
    xxMaterialPtr material = xxMaterial::Create();
    xxMeshPtr mesh = xxMesh::Create(false, 0, 1, 1);
    xxNodePtr node = xxNode::Create();
    xxTexturePtr texture = CreateTexture();

    if (material == nullptr)
        return nullptr;
    if (mesh == nullptr)
        return nullptr;
    if (node == nullptr)
        return nullptr;
    if (texture == nullptr)
        return nullptr;

    int vertex_count = 4;
    mesh->SetVertexCount(vertex_count);
    xxStrideIterator<xxVector3> it_position = mesh->GetPosition();
    (*it_position++) = {-size.x,  size.y, 0};
    (*it_position++) = {-size.x, -size.y, 0};
    (*it_position++) = { size.x,  size.y, 0};
    (*it_position++) = { size.x, -size.y, 0};
    xxStrideIterator<uint32_t> it_color = mesh->GetColor(0);
    (*it_color++) = 0xFFFFFFFF;
    (*it_color++) = 0xFFFFFFFF;
    (*it_color++) = 0xFFFFFFFF;
    (*it_color++) = 0xFFFFFFFF;
    xxStrideIterator<xxVector2> it_texture = mesh->GetTexture(0);
    (*it_texture++) = {-size.x,  size.y};
    (*it_texture++) = {-size.x, -size.y};
    (*it_texture++) = { size.x,  size.y};
    (*it_texture++) = { size.x, -size.y};

    int index_count = 6;
    mesh->SetIndexCount(index_count);
    auto it_index = (uint16_t*)mesh->Storage[xxMesh::INDEX];
    (*it_index++) = 0;
    (*it_index++) = 1;
    (*it_index++) = 2;
    (*it_index++) = 3;
    (*it_index++) = 1;
    (*it_index++) = 2;

    texture->ClampU = false;
    texture->ClampV = false;
    texture->ClampW = false;
    texture->FilterMag = true;
    texture->FilterMin = true;
    texture->FilterMip = true;
    texture->Anisotropic = 16;

    material->Blending = true;
    material->DepthTest = "LessEqual";
    material->DepthWrite = false;
    material->SetTexture(0, texture);

    node->Mesh = mesh;
    node->Material = material;
    node->SetTranslate(translate);

    return node;
}
//------------------------------------------------------------------------------
xxTexturePtr Grid::CreateTexture()
{
    int level = 8;

#if defined(xxWINDOWS)
    uint64_t format = "BGRA8888"_CC;
#else
    uint64_t format = "RGBA8888"_CC;
#endif
    xxTexturePtr texture = xxTexture::Create2D(format, (1 << (level - 1)), (1 << (level - 1)), level);
    if (texture == nullptr)
        return nullptr;

    for (int mipmap = 0; mipmap < level; ++mipmap)
    {
        int width = 1 << (level - mipmap - 1);
        int height = 1 << (level - mipmap - 1);
        uint32_t* pixel = (uint32_t*)(*texture)(0, 0, 0, mipmap, 0);
        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                uint32_t color = 0;
                if (y == 0)
                    color = 0xFF000000;
                if (x == 0)
                    color = 0xFF000000;
                if (width == 4)
                    color &= 0xAA000000;
                if (width == 2)
                    color &= 0x55000000;
                if (width == 1)
                    color &= 0x00000000;
                (*pixel++) = color;
            }
        }
    }

    return texture;
}
//------------------------------------------------------------------------------
