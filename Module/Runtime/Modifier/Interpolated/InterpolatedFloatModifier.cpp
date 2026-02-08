//==============================================================================
// Minamoto : InterpolatedFloatModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "InterpolatedFloatModifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  InterpolatedFloatModifier
//==============================================================================
void InterpolatedFloatModifier::Assign(void* source, float time)
{
    Key key = { time, *(float*)source };
    AssignInterpolated(key);
}
//------------------------------------------------------------------------------
void InterpolatedFloatModifier::Update(void* target, float time, xxModifierData* data)
{
    Key* A;
    Key* B;
    float F;
    if (UpdateInterpolatedFactor(data, time, A, B, F) == false)
        return;

    *(float*)target = Lerp(A->value, B->value, F);
}
//------------------------------------------------------------------------------
xxModifierPtr InterpolatedFloatModifier::Create(size_t count, std::function<void(size_t index, float& time, float& value)> fill)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Key) * count);
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, INTERPOLATED_FLOAT);
    if (fill)
    {
        auto* key = (Key*)modifier->Data.data();
        for (size_t i = 0; i < count; ++i)
        {
            fill(i, key[i].time, key[i].value);
        }
    }
    return modifier;
}
//==============================================================================
