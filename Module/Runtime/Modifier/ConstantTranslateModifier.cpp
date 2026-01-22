//==============================================================================
// Minamoto : ConstantTranslateModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "ConstantTranslateModifier.h"

//==============================================================================
//  ConstantTranslateModifier
//==============================================================================
void ConstantTranslateModifier::Update(void* target, xxModifierData* data, float time)
{
    if (data->time == time)
        return;
    data->time = time;

    auto node = (Node*)target;
    auto* constant = (Constant*)Data.data();
    node->SetTranslate(constant->translate);
}
//------------------------------------------------------------------------------
xxModifierPtr ConstantTranslateModifier::Create(xxVector3 const& translate)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, CONSTANT_TRANSLATE);
    auto* constant = (Constant*)modifier->Data.data();
    constant->translate = translate;
    return modifier;
}
//==============================================================================
