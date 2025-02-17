//==============================================================================
// Minamoto : ShaderCompiler Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include "ShaderDisassemblerAGX.h"
#include "ShaderCompiler.h"

#if defined(xxMACOS) || defined(xxIOS)
#include <Metal/Metal.h>
#include <Metal/MTLBinaryArchive.h>
#endif

//==============================================================================
#if defined(xxMACOS) || defined(xxIOS)
static std::string text =
R"(using namespace metal;

struct Uniform
{
    float4 Position;

    float4x4 World;
    float4x4 View;
    float4x4 Projection;
};

struct Varying
{
    float4 Position [[position]];
    float4 Color0;
    float4 Color1;
    float4 Color2;
    float4 Color3;
    float4 Color4;
    float4 Color5;
    float4 Color6;
    float4 Color7;
    float4 Color10;
    float4 Color11;
    float4 Color12;
    float4 Color13;
    float4 Color14;
    float4 Color15;
    float4 Color16;
    float4 Color17;
};

vertex Varying VSMain
(
    constant Uniform& uni [[buffer(0)]],
    const device float4* pos [[buffer(1)]],
    unsigned int base_instance [[base_instance]],
    unsigned int base_vertex [[base_vertex]],
    unsigned int instance_id [[instance_id]],
    unsigned int vertex_id [[vertex_id]]
)
{
    Varying vary;
    vary.Position = float4(0, 1, 2, 3);
#if 1
    vary.Color0 = float4(4, 5, 6, 7);
    vary.Color1 = float4(8, 9, 10, 11);
    vary.Color2 = float4(12, 13, 14, 15);
    vary.Color3 = float4(16, 17, 18, 19);
    vary.Color4 = float4(20, 21, 22, 23);
    vary.Color5 = float4(24, 25, 26, 27);
    vary.Color6 = float4(28, 29, 30, 31);
    vary.Color7 = float4(32, 33, 34, 35);
    vary.Color10 = float4(36, 37, 38, 39);
    vary.Color11 = float4(40, 41, 42, 43);
    vary.Color12 = float4(44, 45, 46, 47);
    vary.Color13 = float4(48, 49, 50, 51);
    vary.Color14 = float4(52, 53, 54, 55);
    vary.Color15 = float4(56, 57, 58, 59);
    vary.Color16 = float4(60, 61, 62, 63);
    vary.Color17 = float4(64, 65, 66, 67);
#else
    vary.Color0 = pos[vertex_id + 0].x * pos[vertex_id + 0].y;
    vary.Color1 = pos[vertex_id + 1].x * pos[vertex_id + 1].y;
    vary.Color2 = pos[vertex_id + 2].x * pos[vertex_id + 2].y;
    vary.Color3 = pos[vertex_id + 3].x * pos[vertex_id + 3].y;
    vary.Color4 = pos[vertex_id + 4].x * pos[vertex_id + 4].y;
    vary.Color5 = pos[vertex_id + 5].x * pos[vertex_id + 5].y;
    vary.Color6 = pos[vertex_id + 6].x * pos[vertex_id + 6].y;
    vary.Color7 = pos[vertex_id + 7].x * pos[vertex_id + 7].y;
    vary.Color10 = pos[vertex_id + 10].x * pos[vertex_id + 10].y;
    vary.Color11 = pos[vertex_id + 11].x * pos[vertex_id + 11].y;
    vary.Color12 = pos[vertex_id + 12].x * pos[vertex_id + 12].y;
    vary.Color13 = pos[vertex_id + 13].x * pos[vertex_id + 13].y;
    vary.Color14 = pos[vertex_id + 14].x * pos[vertex_id + 14].y;
    vary.Color15 = pos[vertex_id + 15].x * pos[vertex_id + 15].y;
    vary.Color16 = pos[vertex_id + 16].x * pos[vertex_id + 16].y;
    vary.Color17 = pos[vertex_id + 17].x * pos[vertex_id + 17].y;
#endif
    return vary;
}

struct Output
{
    float4 Color [[color(0)]];
//  float Depth [[depth(any)]];
};

fragment Output FSMain
(
    Varying vary [[stage_in]],
    constant Uniform& uni [[buffer(1)]]
)
{
    Output output;
    output.Color = 0;
    output.Color += vary.Color0;
    output.Color += vary.Color1;
    output.Color += vary.Color2;
    output.Color += vary.Color3;
    output.Color += vary.Color4;
    output.Color += vary.Color5;
    output.Color += vary.Color6;
    output.Color += vary.Color7;
    output.Color += vary.Color10;
    output.Color += vary.Color11;
    output.Color += vary.Color12;
    output.Color += vary.Color13;
    output.Color += vary.Color14;
    output.Color += vary.Color15;
    output.Color += vary.Color16;
    output.Color += vary.Color17;
    return output;
})";
#else
static std::string text;
#endif
static std::string vertexDisassembly;
static std::string fragmentDisassembly;
//==============================================================================
static void Compile(int format)
{
#if defined(xxMACOS) || defined(xxIOS)
    NSError* error;
    id <MTLDevice> device = MTLCreateSystemDefaultDevice();
    id <MTLLibrary> library = [device newLibraryWithSource:@(text.c_str())
                                                   options:nil
                                                     error:&error];
    if (error)
    {
        xxLog("ShaderCompiler", "%s", error.localizedDescription.UTF8String);
        return;
    }
    MTLRenderPipelineDescriptor* desc = [MTLRenderPipelineDescriptor new];
    id <MTLBinaryArchive> archive = [device newBinaryArchiveWithDescriptor:[MTLBinaryArchiveDescriptor new]
                                                                     error:&error];
    if (error)
    {
        xxLog("ShaderCompiler", "%s", error.localizedDescription.UTF8String);
        return;
    }
    desc.vertexFunction = [library newFunctionWithName:@"VSMain"];
    desc.fragmentFunction = [library newFunctionWithName:@"FSMain"];
    if (desc.vertexFunction == nil || desc.fragmentFunction == nil)
        return;
    desc.colorAttachments[0].pixelFormat = MTLPixelFormat(format);
    desc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
    [archive addRenderPipelineFunctionsWithDescriptor:desc
                                                error:&error];
    if (error)
    {
        xxLog("ShaderCompiler", "%s", error.localizedDescription.UTF8String);
        return;
    }
    [archive serializeToURL:[NSURL fileURLWithPath:@("/tmp/minamoto.pipeline.bin")]
                      error:&error];
    if (error)
    {
        xxLog("ShaderCompiler", "%s", error.localizedDescription.UTF8String);
        return;
    }
    xxFile* file = xxFile::Load("/tmp/minamoto.pipeline.bin");
    if (file)
    {
        size_t size = file->Size();
        std::vector<uint32_t> archive(size / sizeof(uint32_t));
        file->Read(archive.data(), size);
        delete file;
        remove("/tmp/minamoto.pipeline.bin");

        vertexDisassembly.clear();
        fragmentDisassembly.clear();
        ShaderDisassemblerAGX::Disassemble(archive, [](int found, int gpu, char const* type, void const* binary, size_t size)
        {
            std::string& disassembly = (found == 1) ? vertexDisassembly : fragmentDisassembly;

            std::function<ShaderDisassemblerAGX::Instruction(void const*, size_t)> decode = [gpu]()
            {
                switch (gpu)
                {
                case 'G13X':
                    return ShaderDisassemblerAGX::DecodeG13X;
                case 'G15X':
                    return ShaderDisassemblerAGX::DecodeG15X;
                }
                return ShaderDisassemblerAGX::Decode;
            }();

            char line[128];
            snprintf(line, 128, "%s: %zd\n", type, size);
            disassembly += line;
            for (size_t i = 0; i < size; i += 2)
            {
                unsigned char* code = (unsigned char*)binary + i;
                auto instruction = decode(code, size - i);
                snprintf(line, 128, "%4zd : ", i);
                for (int i = 0; i < instruction.length; i += 2)
                {
                    snprintf(line, 128, "%s%02X%02X", line, code[i], code[i + 1]);
                }
                snprintf(line, 128, "%s%*s", line, 28 - instruction.length * 2 + 2, "");
                disassembly += line;
                disassembly += ShaderDisassemblerAGX::Format(code, instruction);
                disassembly += "\n";
                if (code[0] == 0x0E && code[1] == 0x00)
                    break;
                i += instruction.length ? instruction.length - 2 : 0;
            }
            disassembly += "\n";
        });
    }
#endif
}
//------------------------------------------------------------------------------
bool ShaderCompiler::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    ImGui::SetNextWindowSize(ImVec2(1200.0f, 720.0f), ImGuiCond_Appearing);
    if (ImGui::Begin(ICON_FA_PENCIL_SQUARE_O "Shader Compiler", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        ImVec2 size = ImGui::GetWindowSize();

        bool compile = false;
#if defined(xxMACOS) || defined(xxIOS)
        static int format = MTLPixelFormatRGBA8Unorm;
        if (ImGui::RadioButton("R8", format == MTLPixelFormatR8Unorm))
        {
            format = MTLPixelFormatR8Unorm;
            compile = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("R16", format == MTLPixelFormatR16Float))
        {
            format = MTLPixelFormatR16Float;
            compile = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("R32", format == MTLPixelFormatR32Float))
        {
            format = MTLPixelFormatR32Float;
            compile = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("RGBA8", format == MTLPixelFormatRGBA8Unorm))
        {
            format = MTLPixelFormatRGBA8Unorm;
            compile = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("RGBA16", format == MTLPixelFormatRGBA16Float))
        {
            format = MTLPixelFormatRGBA16Float;
            compile = true;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("RGBA32", format == MTLPixelFormatRGBA32Float))
        {
            format = MTLPixelFormatRGBA32Float;
            compile = true;
        }
#else
        static int format = 0;
#endif

        size.y = size.y - ImGui::GetCursorPosY() - (ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 8.0f);
        float width = size.x / 2.0f;
        float height = size.y / 2.0f;

        compile |= ImGui::InputTextMultiline("INPUT", text, ImVec2(size.x, size.y / 2.0f), ImGuiInputTextFlags_AllowTabInput);
        if (compile)
        {
            Compile(format);
        }

        auto tabWindow = [&](float width, int flag)
        {
            if (ImGui::BeginTabItem("Vertex", nullptr, flag))
            {
                ImGui::InputTextMultiline("", vertexDisassembly.data(), vertexDisassembly.size(), ImVec2(width, height), ImGuiInputTextFlags_ReadOnly);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Fragment", nullptr, flag))
            {
                ImGui::InputTextMultiline("", fragmentDisassembly.data(), fragmentDisassembly.size(), ImVec2(width, height), ImGuiInputTextFlags_ReadOnly);
                ImGui::EndTabItem();
            }
        };

        ImGui::Columns(2);

        ImGui::BeginTabBar("LEFT");
        tabWindow(width - 16.0f, 0);
        ImGui::EndTabBar();

        ImGui::NextColumn();

        ImGui::BeginTabBar("RIGHT");

        static uint64_t focus = 0;
        int flag = 0;
        if (focus != updateData.renderPass)
        {
            focus = updateData.renderPass;
            flag = ImGuiTabItemFlags_SetSelected;
        }

        tabWindow(width - 16.0f, flag);
        ImGui::EndTabBar();

        ImGui::Columns(1);
    }
    ImGui::End();

    return false;
};
//==============================================================================
