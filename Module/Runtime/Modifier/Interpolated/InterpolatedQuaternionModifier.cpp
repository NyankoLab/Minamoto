//==============================================================================
// Minamoto : InterpolatedQuaternionModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "InterpolatedQuaternionModifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  InterpolatedQuaternionModifier
//==============================================================================
void InterpolatedQuaternionModifier::Update(void* target, float time, xxModifierData* data)
{
    Key* A;
    Key* B;
    float F;
    if (UpdateInterpolatedFactor(data, time, A, B, F) == false)
        return;

    auto node = (Node*)target;
    node->SetRotate(xxMatrix3::Quaternion(Lerp((xxVector4&)A->quaternion, (xxVector4&)B->quaternion, F)));
}
//------------------------------------------------------------------------------
xxModifierPtr InterpolatedQuaternionModifier::Create(size_t count, std::function<void(size_t index, float& time, xxVector4& quaternion)> fill)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Key) * count);
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, INTERPOLATED_QUATERNION);
    if (fill)
    {
        auto* key = (Key*)modifier->Data.data();
        for (size_t i = 0; i < count; ++i)
        {
            fill(i, key[i].time, (xxVector4&)key[i].quaternion);
        }
    }
    return modifier;
}
//==============================================================================
