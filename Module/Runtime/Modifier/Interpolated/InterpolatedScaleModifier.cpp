//==============================================================================
// Minamoto : InterpolatedScaleModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "InterpolatedScaleModifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  InterpolatedScaleModifier
//==============================================================================
void InterpolatedScaleModifier::Update(void* target, float time, xxModifierData* data)
{
    Key* A;
    Key* B;
    float F;
    if (UpdateInterpolatedFactor(data, time, A, B, F) == false)
        return;

    auto node = (Node*)target;
    node->SetScale(Lerp(A->scale, B->scale, F));
    node->UpdateRotateTranslateScale();
}
//------------------------------------------------------------------------------
xxModifierPtr InterpolatedScaleModifier::Create(size_t count, std::function<void(size_t index, float& time, float& scale)> fill)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Key) * count);
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, INTERPOLATED_SCALE);
    if (fill)
    {
        auto* key = (Key*)modifier->Data.data();
        for (size_t i = 0; i < count; ++i)
        {
            fill(i, key[i].time, key[i].scale);
        }
    }
    return modifier;
}
//==============================================================================
