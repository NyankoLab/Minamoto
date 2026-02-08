//==============================================================================
// Minamoto : InterpolatedQuaternion16Modifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "InterpolatedQuaternion16Modifier.h"
#include "Modifier/Modifier.inl"

//==============================================================================
//  InterpolatedQuaternion16Modifier
//==============================================================================
void InterpolatedQuaternion16Modifier::Update(void* target, float time, xxModifierData* data)
{
    Key* A;
    Key* B;
    float F;
    if (UpdateInterpolatedFactor(data, time, A, B, F) == false)
        return;

    auto node = (Node*)target;
    xxVector4 L = { __builtin_convertvector((v4hi&)A->quaternion, v4sf) };
    xxVector4 R = { __builtin_convertvector((v4hi&)B->quaternion, v4sf) };
    node->SetRotate(xxMatrix3::Quaternion(Lerp(L, R, F) / INT16_MAX));
}
//------------------------------------------------------------------------------
xxModifierPtr InterpolatedQuaternion16Modifier::Create(size_t count, std::function<void(size_t index, float& time, xxVector4& quaternion)> fill)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Key) * count);
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, INTERPOLATED_QUATERNION16);
    if (fill)
    {
        auto* key = (Key*)modifier->Data.data();
        for (size_t i = 0; i < count; ++i)
        {
            float time;
            xxVector4 quaternion;
            fill(i, time, quaternion);
            quaternion *= INT16_MAX;
            key[i].time = time;
            (v4hi&)key[i].quaternion = __builtin_convertvector(quaternion.v, v4hi);
        }
    }
    return modifier;
}
//==============================================================================
