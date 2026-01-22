//==============================================================================
// Minamoto : Binary Header
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxBinary.h>

class RuntimeAPI Binary : public xxBinary
{
public:
    static xxNodePtr            Load(char const* name);
    static bool                 Save(char const* name, xxNodePtr const& node);

protected:
    Binary();
    virtual ~Binary();

    bool                        Read(void* data, size_t size) override;
    bool                        Write(void const* data, size_t size) override;

    bool                        ReadStream();
    bool                        WriteStream();

    size_t                      m_binaryStreamPosition = 0;
    std::vector<uint8_t>        m_binaryStream;
    std::vector<std::string>    m_stringStream;

public:
    bool                        ReadString(std::string& string) override;
    bool                        WriteString(std::string const& string) override;

    static int constexpr        Current = 0x20241222;
};

#if defined(xxWINDOWS)
#if defined(_LIBCPP_VERSION)
#pragma comment(linker, "/alternatename:?Load@xxBinary@@SA?AV?$shared_ptr@UNode@@@__1@std@@PEBD@Z=?Load@xxBinary@@SA?AV?$shared_ptr@VxxNode@@@__1@std@@PEBD@Z")
#else
#pragma comment(linker, "/alternatename:?Load@xxBinary@@SA?AV?$shared_ptr@UNode@@@std@@PEBD@Z=?Load@xxBinary@@SA?AV?$shared_ptr@VxxNode@@@std@@PEBD@Z")
#endif
#endif
