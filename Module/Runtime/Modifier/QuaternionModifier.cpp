//==============================================================================
// Minamoto : QuaternionModifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Graphic/Node.h"
#include "QuaternionModifier.h"

//==============================================================================
//  QuaternionModifier
//==============================================================================
void QuaternionModifier::Update(void* target, float time, xxModifierData* data)
{
    if (data->time == time)
        return;
    data->time = time;

    auto node = (Node*)target;
    auto* constant = (Constant*)Data.data();
    node->SetRotate(xxMatrix3::Quaternion(constant->quaternion));
}
//------------------------------------------------------------------------------
xxModifierPtr QuaternionModifier::Create(xxVector4 const& quaternion)
{
    xxModifierPtr modifier = xxModifier::Create(sizeof(Constant));
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, QUATERNION);
    auto* constant = (Constant*)modifier->Data.data();
    constant->quaternion = quaternion;
    return modifier;
}
//==============================================================================
