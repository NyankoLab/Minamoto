//==============================================================================
// Minamoto : ConstantQuaternionModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "ConstantQuaternionModifier.h"

//==============================================================================
//  ConstantQuaternionModifier
//==============================================================================
void ConstantQuaternionModifier::Update(void* target, xxModifierData* data, float time)
{
    if (data->time == time)
        return;
    data->time = time;

    auto node = (Node*)target;
    auto* constant = (Constant*)Data.data();
    node->SetRotate(xxMatrix3::Quaternion(constant->quaternion));
}
//------------------------------------------------------------------------------
xxModifierPtr ConstantQuaternionModifier::Create(xxVector4 const& quaternion)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, CONSTANT_QUATERNION);
    auto* constant = (Constant*)modifier->Data.data();
    constant->quaternion = quaternion;
    return modifier;
}
//==============================================================================
