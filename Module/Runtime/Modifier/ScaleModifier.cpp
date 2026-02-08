//==============================================================================
// Minamoto : ScaleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "ScaleModifier.h"

//==============================================================================
//  ScaleModifier
//==============================================================================
void ScaleModifier::Update(void* target, float time, xxModifierData* data)
{
    if (data->time == time)
        return;
    data->time = time;

    auto node = (Node*)target;
    auto* constant = (Constant*)Data.data();
    node->SetScale(constant->scale);
    node->UpdateRotateTranslateScale();
}
//------------------------------------------------------------------------------
xxModifierPtr ScaleModifier::Create(float scale)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, SCALE);
    auto* constant = (Constant*)modifier->Data.data();
    constant->scale = scale;
    return modifier;
}
//==============================================================================
