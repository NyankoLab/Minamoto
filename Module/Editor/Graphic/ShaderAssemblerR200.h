//==============================================================================
// Minamoto : ShaderAssemblerR200 Header
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

struct ShaderAssemblerR200
{
    static std::vector<uint32_t> CompileChaplinPVS(std::vector<uint32_t> const& shader, std::string& message);
    static std::vector<uint32_t> CompileChaplin(std::vector<uint32_t> const& shader, std::string& message);
    static std::string DisassembleChaplinPVS(std::vector<uint32_t> const& code);
    static std::string DisassembleChaplin(std::vector<uint32_t> const& code);
};
