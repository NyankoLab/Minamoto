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
    static int InstructionLength(void const* data, size_t size);
    static int InstructionLengthG13X(void const* data, size_t size);
    static int InstructionLengthG15X(void const* data, size_t size);
};
