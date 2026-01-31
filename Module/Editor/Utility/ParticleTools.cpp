//==============================================================================
// Minamoto : ParticleTools Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Node.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Modifier/Particle/RainParticleModifier.h>
#include "ParticleTools.h"

#define TAG "ParticleTools"

//==============================================================================
xxNodePtr ParticleTools::CreateParticle(int type)
{
    xxMaterialPtr material = Material::Create();
    xxMeshPtr mesh = Mesh::Create(false, 0, 1, 1);
    xxModifierPtr modifier = RainParticleModifier::Create();
    xxNodePtr node = Node::Create();
    xxTexturePtr texture = CreateTexture();

    if (material == nullptr)
        return nullptr;
    if (mesh == nullptr)
        return nullptr;
    if (modifier == nullptr)
        return nullptr;
    if (node == nullptr)
        return nullptr;
    if (texture == nullptr)
        return nullptr;

    texture->ClampU = false;
    texture->ClampV = false;
    texture->ClampW = false;
    texture->FilterMag = true;
    texture->FilterMin = true;
    texture->FilterMip = true;

    material->Blending = true;
    material->BlendSourceColor = "SrcAlpha";
    material->BlendDestinationColor = "One";
    material->DepthTest = "LessEqual";
    material->DepthWrite = false;
    material->SetTexture(0, texture);

    node->Flags |= Node::PARTICLE;
    node->Mesh = mesh;
    node->Material = material;
    node->Modifiers.emplace_back(modifier);

    auto parameter = (RainParticleModifier::Parameter*)modifier->Data.data();
    parameter->now = 0;
    parameter->count = 100;
    parameter->start = 0.0f;
    parameter->birth = 1.0f;
    parameter->life = 10.0f;
    parameter->width = 10.0f;
    parameter->height = 10.0f;
    parameter->size = 2.0f;
    parameter->speed = 10.0f;
    parameter->variation = 0.005f;

    return node;
}
//------------------------------------------------------------------------------
xxTexturePtr ParticleTools::CreateTexture()
{
#if defined(xxWINDOWS)
    uint64_t format = "BGRA8888"_CC;
#else
    uint64_t format = "RGBA8888"_CC;
#endif
    xxTexturePtr texture = xxTexture::Create2D(format, 256, 256, 1);
    if (texture == nullptr)
        return nullptr;

    char* pixel = (char*)(*texture)(0, 0, 0, 0, 0);
    for (int y = 0; y < 256; ++y)
    {
        for (int x = 0; x < 256; ++x)
        {
            float dx = (x + 0.5f) - 128;
            float dy = (y + 0.5f) - 128;
            float dist = sqrtf(dx * dx + dy * dy) / 96;
            float alpha = std::clamp(expf(-dist * 4.0f), 0.0f, 1.0f);

            (*pixel++) = -1;
            (*pixel++) = -1;
            (*pixel++) = -1;
            (*pixel++) = (char)(alpha * 255.0f);
        }
    }

    return texture;
}
//==============================================================================
