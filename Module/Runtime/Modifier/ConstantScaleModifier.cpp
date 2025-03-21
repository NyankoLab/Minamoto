//==============================================================================
// Minamoto : ConstantScaleModifier Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "ConstantScaleModifier.h"

//==============================================================================
//  ConstantScaleModifier
//==============================================================================
void ConstantScaleModifier::Update(void* target, xxModifierData* data, float time)
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
xxModifierPtr ConstantScaleModifier::Create(float scale)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, CONSTANT_SCALE);
    auto* constant = (Constant*)modifier->Data.data();
    constant->scale = scale;
    return modifier;
}
//==============================================================================
