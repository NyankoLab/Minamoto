//==============================================================================
// Minamoto : Float4Modifier Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Float4Modifier.h"

//==============================================================================
//  Float4Modifier
//==============================================================================
void Float4Modifier::Set(xxVector4 const& value)
{
    Data.assign((char*)&value, (char*)&value + sizeof(xxVector4));
}
//------------------------------------------------------------------------------
xxVector4 Float4Modifier::Get() const
{
    xxVector4 value = xxVector4::ZERO;
    memcpy(&value, Data.data(), std::min(Data.size(), sizeof(xxVector4)));
    return value;
}
//------------------------------------------------------------------------------
void Float4Modifier::Assign(void* source, float time)
{
    Set(*(xxVector4*)source);
}
//------------------------------------------------------------------------------
void Float4Modifier::Update(void* target, float time, xxModifierData* data)
{
    *(xxVector4*)target = Get();
}
//------------------------------------------------------------------------------
xxModifierPtr Float4Modifier::Create(xxVector4 const& value)
{
    xxModifierPtr modifier = xxModifier::Create();
    if (modifier == nullptr)
        return nullptr;

    Loader(*modifier, FLOAT4);
    auto ptr = (std::shared_ptr<Float4Modifier> const&)modifier;
    ptr->Set(value);

    return modifier;
}
//==============================================================================
