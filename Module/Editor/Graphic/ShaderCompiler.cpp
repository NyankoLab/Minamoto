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
R"(struct Varying
{
    float4 Position [[position]];
};

vertex Varying VSMain()
{
    Varying vary;
    vary.Position = float4(0, 0, 0, 0);
    return vary;
}

struct Output
{
    float4 Color [[color(0)]];
//  float Depth [[depth(any)]];
};

fragment Output FSMain()
{
    Output output;
    output.Color = float4(0, 0, 0, 0);
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

            std::function<int(void const*, size_t)> instruction_length = [gpu]()
            {
                switch (gpu)
                {
                case 'G13X':
                    return ShaderDisassemblerAGX::InstructionLengthG13X;
                case 'G15X':
                    return ShaderDisassemblerAGX::InstructionLengthG15X;
                }
                return ShaderDisassemblerAGX::InstructionLength;
            }();

            char line[128];
            snprintf(line, 128, "%s: %zd\n", type, size);
            disassembly += line;
            for (size_t i = 0; i < size; i += 2)
            {
                unsigned char* code = (unsigned char*)binary + i;
                int length = instruction_length(code, size - i);
                snprintf(line, 128, "%4zd : ", i);
                for (int i = 0; i < length; i += 2)
                {
                    snprintf(line, 128, "%s%02X%02X", line, code[i], code[i + 1]);
                }
                disassembly += line;
                disassembly += "\n";
                if (code[0] == 0x0E && code[1] == 0x00)
                    break;
                i += length ? length - 2 : 0;
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

        static bool focus = false;
        int flag = 0;
        if (focus == false)
        {
            focus = true;
            flag = ImGuiTabItemFlags_SetSelected;
        }

        ImGui::BeginTabBar("RIGHT");
        tabWindow(width - 16.0f, flag);
        ImGui::EndTabBar();

        ImGui::Columns(1);
    }
    ImGui::End();

    return false;
};
//==============================================================================
