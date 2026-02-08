//==============================================================================
// Minamoto : TranslateModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "TranslateModifier.h"

//==============================================================================
//  TranslateModifier
//==============================================================================
void TranslateModifier::Update(void* target, float time, xxModifierData* data)
{
    if (data->time == time)
        return;
    data->time = time;

    auto node = (Node*)target;
    auto* constant = (Constant*)Data.data();
    node->SetTranslate(constant->translate);
}
//------------------------------------------------------------------------------
xxModifierPtr TranslateModifier::Create(xxVector3 const& translate)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, TRANSLATE);
    auto* constant = (Constant*)modifier->Data.data();
    constant->translate = translate;
    return modifier;
}
//==============================================================================
