//==============================================================================
// Minamoto : ShaderDisassemblerAGX Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include "ShaderDisassemblerAGX.h"

//==============================================================================
void ShaderDisassemblerAGX::Disassemble(std::vector<uint32_t> const& archive, std::function<void(int, int, char const*, void const*, size_t)> callback)
{
    int found = 0;
    for (size_t i = 0; i < archive.size(); ++i)
    {
        uint32_t magic = archive[i];
        if (magic != 0xFEEDFACF)
            continue;

        if (found == 0)
        {
            found++;
            continue;
        }

        int gpu = 'G13X';
        uint32_t size = archive[i + 0x90 / sizeof(uint32_t)];
        uint32_t offset = archive[i + 0x98 / sizeof(uint32_t)];
        size_t offset_prolog = i * sizeof(uint32_t) + offset;
        size_t offset_main = SIZE_MAX;
        size_t size_prolog = size;
        size_t size_main = 0;
        for (size_t i = offset_prolog / sizeof(uint32_t); i < (offset_prolog + size) / sizeof(uint32_t); ++i)
        {
            uint32_t code = archive[i];
            if (code == 'HASH')
                break;
            if (code == 0x00080008 || code == 0x00060006)
            {
                size_prolog = i * sizeof(uint32_t) - offset_prolog;

                if (code == 0x00060006)
                    gpu = 'G15X';

                for (size_t j = i; j < (offset_prolog + size) / sizeof(uint32_t); ++j)
                {
                    code = archive[j];
                    if (code == 'HASH')
                        break;
                    if (code != 0x00080008 && code != 0x00060006)
                    {
                        offset_main = j * sizeof(uint32_t);
                        size_main = size - (offset_main - offset_prolog);

                        for (size_t k = j; k < (offset_prolog + size) / sizeof(uint32_t); ++k)
                        {
                            uint32_t code = archive[k];
                            if (code == 'HASH')
                                break;
                            if (code == 0x00080008 || code == 0x00060006)
                            {
                                size_main = k * sizeof(uint32_t) - offset_main;
                                break;
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }

        if (found == 1 || found == 2)
        {
            if (offset_main == SIZE_MAX)
            {
                callback(found, gpu, "main", (char*)archive.data() + offset_prolog, size_prolog);
            }
            else
            {
                callback(found, gpu, "prolog", (char*)archive.data() + offset_prolog, size_prolog);
                callback(found, gpu, "main", (char*)archive.data() + offset_main, size_main);
            }

            found++;
        }
    }
}
//------------------------------------------------------------------------------
std::pair<int, char const*> ShaderDisassemblerAGX::Instruction(void const* data, size_t size)
{
    return { 2, "???" };
}
//------------------------------------------------------------------------------
std::pair<int, char const*> ShaderDisassemblerAGX::InstructionG13X(void const* data, size_t size)
{
    // https://github.com/dougallj/applegpu
    unsigned char code[12] = {};
    memcpy(code, data, size < 12 ? size : 12);
    bool L = (code[1] & 0x80) == 0x80;
    switch (code[0] & 7)
    {
    case 0:
        switch (code[0])
        {
        case 0b00000000: return { L ? 6 : 4, L ? "jmp_exec_any" : "jmp_incomplete" };
        case 0b00001000: return { 2, "trap" };
        case 0b00010000: return { 6, "call" };
        case 0b00100000: return { 6, "jmp_exec_none" };
        case 0b00111000: return { 2, "wait" };
        case 0b01001000: return { 4, "wait_pix" };
        case 0b01011000: return { 4, "signal_pix" };
        case 0b01101000: return { 2, "threadgroup_barrier" };
        case 0b10001000: return { 2, "stop" };
        }
        return { 2, "???" };
    case 1:
        switch (code[0] & 0b1111001)
        {
//      case 0b0000001: return { 8, "???" };
        case 0b0001001: return { 8, "st_tile" };
//      case 0b0010001: return { 8, "???" };
//      case 0b0011001: return { 8, "???" };
        case 0b0100001: return { L ? 8 : 4, "ld_var" };
        case 0b0101001: return { L ? 8 : 6, "threadgroup_store" };
        case 0b0110001: return { L ? 12 : 8, "texture_sample" };
        case 0b0111001: return { L ? 8 : 6, "threadgroup_store" };
//      case 0b1000001: return { 8, "???" };
        case 0b1001001: return { 8, "ld_tile" };
//      case 0b1010001: return { 8, "???" };
//      case 0b1011001: return { 8, "???" };
        case 0b1100001: return { L ? 8 : 4, "ld_var" };
        case 0b1101001: return { L ? 8 : 6, "threadgroup_load" };
        case 0b1110001: return { L ? 12 : 8, "texture_load" };
        case 0b1111001: return { L ? 8 : 6, "threadgroup_load" };
        }
        return { 8, "???" };
    case 2:             // basic 32bit operation
        switch (code[0] & 0b1111010)
        {
        case 0b0000010: return { L ? 10 : 8, "fcmpsel" };
        case 0b0001010: return { L ? 6 : 4, "floor" };      // TODO
        case 0b0010010: return { L ? 10 : 8, "icmpsel" };
        case 0b0011010: return { 6, "fmul" };
        case 0b0100010: return { 8, "fcmp_ballot" };        // TODO
        case 0b0101010: return { 6, "fadd" };
        case 0b0110010: return { 8, "icmp_ballot" };
        case 0b0111010: return { 8, "fmadd" };
        case 0b1000010: return { 6, "if_fcmp" };            // TODO
        case 0b1001010: return { L ? 6 : 4, "floor_sat" };  // TODO
        case 0b1010010: return { 6, "pop_exec" };           // TODO
        case 0b1011010: return { 6, "fmul_sat" };
        case 0b1100010:
            switch (code[1] & 1)
            {
            case 0: return { L ? 6 : 4, "mov" };
            case 1: return { L ? 8 : 6, "mov" };
            }
            return { L ? 6 : 4, "mov" };
        case 0b1101010: return { 6, "fadd_sat" };
        case 0b1110010: return { 4, "get_sr" };
        case 0b1111010: return { 8, "fmadd_sat" };
        }
        return { 2, "???" };
//  case 3: return { 2, "???" };
    case 4:
        switch (code[0] & 0b1111100)
        {
        case 0b0000100: return { 2, "call" };
        case 0b0010100: return { 2, "ret" };
        }
        return { 2, "???" };
    case 5:
        L = (code[5] & 0x80) == 0x80;
        switch (code[0] & 0b11111101)
        {
        case 0b00000101: return { L ? 8 : 6, "device_load" };   // TODO
        case 0b00110101: return { L ? 8 : 6, "stack_load" };    // TODO
        case 0b01000101: return { L ? 8 : 6, "device_store" };  // TODO
        case 0b10000101: return { L ? 8 : 6, "device_load" };   // TODO
        case 0b10110101: return { L ? 8 : 6, "stack_store" };   // TODO
        case 0b11000101: return { L ? 8 : 6, "device_store" };  // TODO
        }
        return { L ? 8 : 6, "???" };
    case 6:             // basic 16bit operation
        switch (code[0] & 0b1111110)
        {
//      case 0b0000110: return { 2, "???" };
        case 0b0001110: return { 8, "iadd" };
        case 0b0010110: return { 6, "fmul16" };
        case 0b0011110: return { 8, "imadd" };
        case 0b0100110: return { 6, "fadd16" };
        case 0b0101110: return { 8, "bitfield" };
        case 0b0110110: return { L ? 8 : 6, "fmadd16" };
        case 0b0111110: return { 6, L ? "convert" : "bitwise" };
//      case 0b1000110: return { 2, "???" };
        case 0b1001110: return { 8, "iadd_sat" };
        case 0b1010110: return { 6, "fmul16_sat" };
        case 0b1011110: return { 8, "imadd_sat" };
        case 0b1100110: return { 6, "fadd16_sat" };
//      case 0b1101110: return { 2, "???" };
        case 0b1110110: return { L ? 8 : 6, "fmadd16_sat" };
        case 0b1111110: return { 6, "bitop" };
        }
        return { 2, "???" };
    case 7: return { 6, "???" };
    }
    return { 2, "???" };
}
//------------------------------------------------------------------------------
std::pair<int, char const*> ShaderDisassemblerAGX::InstructionG15X(void const* data, size_t size)
{
    // https://github.com/TellowKrinkle/applegpu/tree/M3
    unsigned char code[16] = {};
    memcpy(code, data, size < 16 ? size : 16);
    switch (code[0] & 7)
    {
    case 0:
    case 1:
        switch (code[2] & 7)
        {
        case 0:
        case 1:
        case 2:
        case 3: return { 4, "???" };
        case 5:
            switch (code[4] & 2)
            {
            case 2: return { 12, "???" };
            }
        }
        return { 6 + 2 * (code[4] & 3), "???" };
    case 2:
        switch (code[2] & 1)
        {
        case 0: return { 6, "???" };
        case 1:
            switch (code[4] & 3)
            {
            case 0:
            case 1: return { 8, "???" };
            case 2: return { 10, "???" };
            }
        }
        return { 14, "???" };
    case 3:
        switch (code[2] & 6)
        {
        case 0:
        case 2:
        case 4: return { 4, "???" };
        }
        return { 10, "???" };
    case 4:
        switch (code[1] & 0x80)
        {
        case 0x00: return { 2, "???" };
        default:
            switch (code[2] & 3)
            {
            case 0x0: return { 4, "???" };
            case 0x1:
            case 0x2: return { 8, "???" };
            }
            return { 10, "???" };
        }
        return { 10, "???" };
    case 5:
        return { 14, "???" };
    case 6:
        switch (code[1] & 1)
        {
        case 0: return { 4, "???" };
        }
        return { 8, "???" };
    case 7:
        switch ((code[0] << 8) | (code[1] & 0x0F))
        {
            case 0x0f04: return { 4, "else" };
            case 0x0f05: return { 4, "if" };
            case 0x0704: return { 6, "barrier" };
            case 0x5701: return { 6, "jmp_exec_none" };
            case 0x0f06: return { 6, "pop_exec" };
            case 0x8702: return { 6, "???" };
            case 0x1704: return { 8, "unpack unorm/snorm" };
            case 0x2705: return { 8, "popcount" };
            case 0x2706: return { 8, "unpack rgb10a2 rg11b10f rgb9e5" };
            case 0xa707: return { 8, "convert" };
            case 0xa704: return { 8, "bitrev" };
            case 0xa705: return { 8, "ffs" };
            case 0x5706: return { 8, "store vertex output" };
            case 0x3700: return { 8, "quad_and" };
            case 0x3701: return { 8, "quad_xor" };
            case 0x3702: return { 8, "quad_smin" };
            case 0x3703: return { 8, "quad_umin" };
            case 0x3706: return { 8, "quad_fadd" };
            case 0xb700: return { 8, "quad_or" };
            case 0xb701: return { 8, "quad_iadd" };
            case 0xb702: return { 8, "quad_smax" };
            case 0xb703: return { 8, "quad_umax" };
            case 0xb706: return { 8, "quad_fmul" };
            case 0xb705: return { 8, "quad_fmin" };
            case 0xb707: return { 8, "quad_fmax" };
            case 0x3f00: return { 8, "simd_and" };
            case 0x3f01: return { 8, "simd_xor" };
            case 0x3f02: return { 8, "simd_smin" };
            case 0x3f03: return { 8, "simd_umin" };
            case 0x3f06: return { 8, "simd_fadd" };
            case 0xbf00: return { 8, "simd_or" };
            case 0xbf01: return { 8, "simd_iadd" };
            case 0xbf02: return { 8, "simd_smax" };
            case 0xbf03: return { 8, "simd_umax" };
            case 0xbf06: return { 8, "simd_fmul" };
            case 0xbf05: return { 8, "simd_fmin" };
            case 0xbf07: return { 8, "simd_fmax" };
            case 0x0f01: return { 10, "jmp_exec_none?" };
            case 0x2707: return { 10, "unknown, appears in round implementation" };
            case 0x2f00: return { 10, "floor/ceil/trunc/rint" };
            case 0x2f02: return { 10, "log2" };
            case 0x2f03: return { 10, "sin_pt_1???" };
            case 0x9f01: return { 10, "iadd" };
            case 0x9704: return { 10, "pack unorm/snorm" };
            case 0x970c: return { 10, "???" };
            case 0x1f01: return { 10, "isub" };
            case 0x2703: return { 10, "???" };
            case 0x2704: return { 10, "???" };
            case 0xa701: return { 10, "asr" };
            case 0xa703: return { 10, "asrh" };
            case 0xaf00: return { 10, "rcp" };
            case 0xaf02: return { 10, "exp2" };
            case 0x4700: return { 10, "quad_shuffle" };
            case 0x4701: return { 10, "quad_shuffle_up" };
            case 0xc701: return { 10, "quad_shuffle_down" };
            case 0xc700: return { 10, "quad_shuffle_xor" };
            case 0x4704: return { 10, "simd_shuffle" };
            case 0x4705: return { 10, "simd_shuffle_up" };
            case 0xc705: return { 10, "simd_shuffle_down" };
            case 0xc704: return { 10, "simd_shuffle_xor" };
            case 0x1702: return { 10, "quad_ballot" };
            case 0x1707: return { 10, "simd_ballot" };
            case 0x2700: return { 12, "bfi" };
            case 0x2701: return { 12, "extr" };
            case 0x2702: return { 12, "shlhi" };
            case 0x9f00: return { 12, "imadd" };
            case 0x1f00: return { 12, "imsub" };
            case 0xa700: return { 12, "bfeil" };
            case 0xa702: return { 12, "shrhi" };
            case 0x9703: return { 12, "quad_ballot?" };
            case 0x9707: return { 12, "simd_ballot?" };
            case 0x6f00: return { 12, "???" };
            case 0x1705: return { 12, "???" };
            case 0x8f04: return { 14, "while + jmp_exec_any?" };
            case 0x6700: return { 14, "device_load" };
            case 0x6702: return { 14, "threadgroup_load" };
            case 0xa706: return { 14, "pack rgb10a2 rg11b10f rgb9e5" };
            case 0xe700: return { 14, "device_store" };
            case 0xe702: return { 14, "theadgroup_store" };
                case 0xe706: return { 14, "???" };
            case 0xd700: return { 16, "image_store" };
        }
        return { 2, "???" };
    }
    return { 2, "???" };
}
//==============================================================================
