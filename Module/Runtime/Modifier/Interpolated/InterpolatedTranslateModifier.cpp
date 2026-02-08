//==============================================================================
// Minamoto : TranslateModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "InterpolatedTranslateModifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  InterpolatedTranslateModifier
//==============================================================================
void InterpolatedTranslateModifier::Update(void* target, float time, xxModifierData* data)
{
    Key* A;
    Key* B;
    float F;
    if (UpdateInterpolatedFactor(data, time, A, B, F) == false)
        return;

    auto node = (Node*)target;
    node->SetTranslate(Lerp(A->translate, B->translate, F));
}
//------------------------------------------------------------------------------
xxModifierPtr InterpolatedTranslateModifier::Create(size_t count, std::function<void(size_t index, float& time, xxVector3& translate)> fill)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Key) * count);
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, INTERPOLATED_TRANSLATE);
    if (fill)
    {
        auto* key = (Key*)modifier->Data.data();
        for (size_t i = 0; i < count; ++i)
        {
            fill(i, key[i].time, key[i].translate);
        }
    }
    return modifier;
}
//==============================================================================
