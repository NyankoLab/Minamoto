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
    static std::pair<int, char const*> Instruction(void const* data, size_t size);
    static std::pair<int, char const*> InstructionG13X(void const* data, size_t size);
    static std::pair<int, char const*> InstructionG15X(void const* data, size_t size);
};
