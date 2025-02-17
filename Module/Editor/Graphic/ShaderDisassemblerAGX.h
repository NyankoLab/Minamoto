//==============================================================================
// Minamoto : ShaderDisassemblerAGX Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

struct ShaderDisassemblerAGX
{
    static void Disassemble(std::vector<uint32_t> const& archive, std::function<void(int, int, char const*, void const*, size_t)> callback);

    struct Instruction
    {
        int length;
        char const* name;
        char const* parameter;
    };

    static Instruction Decode(void const* data, size_t size);
    static Instruction DecodeG13X(void const* data, size_t size);
    static Instruction DecodeG15X(void const* data, size_t size);
    static std::string Format(void const* data, Instruction instruction);
};
