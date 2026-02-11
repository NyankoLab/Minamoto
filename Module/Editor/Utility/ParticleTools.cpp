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
#include <Runtime/Modifier/Particle/SprayParticleModifier.h>
#include <Runtime/Modifier/Particle/SuperSprayParticleModifier.h>
#include "ParticleTools.h"
#include "TextureTools.h"

#define TAG "ParticleTools"

//==============================================================================
xxNodePtr ParticleTools::CreateParticle(uint64_t type)
{
#if HAVE_PARTICLE
    xxMaterialPtr material = Material::Create();
    xxMeshPtr mesh = Mesh::Create(false, 0, 1, 1);
    xxModifierPtr modifier;
    xxNodePtr node = Node::Create();
    xxTexturePtr texture = TextureTools::CreateStarTexture();

    switch (type)
    {
    case "Spray"_CC:        modifier = SprayParticleModifier::Create();      break;
    case "SuperSpray"_CC:   modifier = SuperSprayParticleModifier::Create(); break;
    }

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

    switch (type)
    {
    case "Spray"_CC:
    {
        auto parameter = (SprayParticleModifier::Parameter*)modifier->Data.data();
        parameter->now = 0;
        parameter->count = 100;
        parameter->start = 0.0f;
        parameter->birth = 1.0f;
        parameter->life = 30.0f;
        parameter->range.x = 0.0f;
        parameter->range.y = 0.0f;
        parameter->size = 2.0f;
        parameter->speed = 10.0f;
        parameter->variation = 0.02f;
        parameter->CalculateBound();
        break;
    }
    case "SuperSpray"_CC:
    {
        auto parameter = (SuperSprayParticleModifier::Parameter*)modifier->Data.data();
        parameter->now = 0;
        parameter->count = 100;
        parameter->offset.x = 0.0f;
        parameter->spread.x = 0.0f;
        parameter->offset.y = 0.0f;
        parameter->spread.y = 0.0f;
        parameter->speed = 10.0f;
        parameter->speedVariation = 0.0f;
        parameter->start = 0.0f;
        parameter->stop = 30.0f;
        parameter->life = 30.0f;
        parameter->lifeVariation = 0.0f;
        parameter->size = 1.0f;
        parameter->sizeVariation = 0.0f;
        parameter->grow = 0.0f;
        parameter->fade = 0.0f;
        parameter->spin = 0.0f;
        parameter->spinVariation = 0.0f;
        parameter->phase = 0.0f;
        parameter->phaseVariation = 0.0f;
        parameter->range.x = 0.0f;
        parameter->range.y = 0.0f;
        parameter->CalculateBound();
        break;
    }
    default:
        break;
    }

    return node;
#else
    return nullptr;
#endif
}
//------------------------------------------------------------------------------
void ParticleTools::ConvertParticle(xxModifier& modifier, uint64_t type)
{
#if HAVE_PARTICLE
    SprayParticleModifier::Parameter sprayParameter;
    SuperSprayParticleModifier::Parameter superSprayParameter;

    switch (modifier.DataType)
    {
    case Modifier::SPRAY_PARTICLE:
        sprayParameter = *(SprayParticleModifier::Parameter*)modifier.Data.data();
        break;
    case Modifier::SUPERSPRAY_PARTICLE:
        superSprayParameter = *(SuperSprayParticleModifier::Parameter*)modifier.Data.data();
        break;
    }

    switch (type)
    {
    case "Spray"_CC:
    {
        Modifier::Loader(modifier, Modifier::SPRAY_PARTICLE);
        modifier.Data.assign(sizeof(SprayParticleModifier::Parameter), 0);
        auto parameter = (SprayParticleModifier::Parameter*)modifier.Data.data();
        parameter->now = 0;
        parameter->count = 100;
        parameter->start = 0.0f;
        parameter->birth = 1.0f;
        parameter->life = 30.0f;
        parameter->range.x = 0.0f;
        parameter->range.y = 0.0f;
        parameter->size = 2.0f;
        parameter->speed = 10.0f;
        parameter->variation = 0.02f;
        parameter->CalculateBound();
        break;
    }
    case "SuperSpray"_CC:
    {
        Modifier::Loader(modifier, Modifier::SUPERSPRAY_PARTICLE);
        modifier.Data.assign(sizeof(SuperSprayParticleModifier::Parameter), 0);
        auto parameter = (SuperSprayParticleModifier::Parameter*)modifier.Data.data();
        parameter->now = 0;
        parameter->count = 100;
        parameter->offset.x = 0.0f;
        parameter->spread.x = 0.0f;
        parameter->offset.y = 0.0f;
        parameter->spread.y = 0.0f;
        parameter->speed = 10.0f;
        parameter->speedVariation = 0.0f;
        parameter->start = 0.0f;
        parameter->stop = 30.0f;
        parameter->life = 30.0f;
        parameter->lifeVariation = 0.0f;
        parameter->size = 1.0f;
        parameter->sizeVariation = 0.0f;
        parameter->grow = 0.0f;
        parameter->fade = 0.0f;
        parameter->spin = 0.0f;
        parameter->spinVariation = 0.0f;
        parameter->phase = 0.0f;
        parameter->phaseVariation = 0.0f;
        parameter->range.x = 0.0f;
        parameter->range.y = 0.0f;
        parameter->CalculateBound();
        break;
    }
    default:
        break;
    }
#endif
}
//==============================================================================
