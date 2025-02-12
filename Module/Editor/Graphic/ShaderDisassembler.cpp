//==============================================================================
// Minamoto : PipelineDisassembler Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <xxGraphicPlus/xxFile.h>
#include "ShaderDisassemblerAGX.h"
#include "ShaderDisassembler.h"

#if defined(xxWINDOWS)
#include <unknwn.h>
struct D3DBlob : public IUnknown
{
    virtual LPCSTR STDMETHODCALLTYPE GetBufferPointer() = 0;
    virtual SIZE_T STDMETHODCALLTYPE GetBufferSize() = 0;
};
#elif defined(xxMACOS) || defined(xxIOS)
#include <Metal/Metal.h>
#include <Metal/MTLBinaryArchive.h>
#endif

//==============================================================================
struct ShaderDisassemblyData
{
    uint64_t device = 0;
    uint64_t blendState = 0;
    uint64_t vertexAttribute = 0;
    uint64_t meshShader = 0;
    uint64_t vertexShader = 0;
    uint64_t fragmentShader = 0;
    std::string meshDisassembly;
    std::string vertexDisassembly;
    std::string fragmentDisassembly;
    bool disassembly = false;
};
static std::map<uint64_t, ShaderDisassemblyData> allShaderDisassembly;
//------------------------------------------------------------------------------
static uint64_t (*xxCreatePipelineSystem)(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader);
//------------------------------------------------------------------------------
static uint64_t xxCreatePipelineRuntime(uint64_t device, uint64_t renderPass, uint64_t blendState, uint64_t depthStencilState, uint64_t rasterizerState, uint64_t vertexAttribute, uint64_t meshShader, uint64_t vertexShader, uint64_t fragmentShader)
{
    uint64_t output = xxCreatePipelineSystem(device, renderPass, blendState, depthStencilState, rasterizerState, vertexAttribute, meshShader, vertexShader, fragmentShader);
    if (output)
    {
        allShaderDisassembly[output] = {device, blendState, vertexAttribute, meshShader, vertexShader, fragmentShader};
    }
    return output;
}
//------------------------------------------------------------------------------
static void Disassemble(ShaderDisassemblyData& data)
{
    if (data.disassembly)
        return;
    data.disassembly = true;
#if defined(xxWINDOWS)
    if (strstr(xxGetInstanceName(), "Direct3D"))
    {
        void* d3dcompiler = xxLoadLibrary("d3dcompiler_47.dll");
        if (d3dcompiler)
        {
            HRESULT (WINAPI* D3DDisassemble)(void const*, SIZE_T, int, char const*, D3DBlob**) = nullptr;
            (void*&)D3DDisassemble = xxGetProcAddress(d3dcompiler, "D3DDisassemble");
            if (D3DDisassemble)
            {
                auto disassemble = [&](uint64_t shader, std::string& disassembly)
                {
                    DWORD const* code = reinterpret_cast<DWORD const*>(shader);
                    if (code == nullptr)
                        return;
                    D3DBlob* text = nullptr;
                    if (code[0] == "DXBC"_cc)
                    {
                        D3DDisassemble(code, code[6], 0, "", &text);
                    }
                    else
                    {
                        size_t size = xxAllocSize(code);
                        D3DDisassemble(code, size, 0, "", &text);
                    }
                    if (text)
                    {
                        disassembly.assign((char*)text->GetBufferPointer(), text->GetBufferSize());
                        text->Release();
                    }
                };
                disassemble(data.meshShader, data.meshDisassembly);
                disassemble(data.vertexShader, data.vertexDisassembly);
                disassemble(data.fragmentShader, data.fragmentDisassembly);
            }
            xxFreeLibrary(d3dcompiler);
        }
    }
#elif defined(xxMACOS) || defined(xxIOS)
    if (strstr(xxGetInstanceName(), "Metal"))
    {
        if (@available(macOS 12, *))
        {
            NSError* error;
            id <MTLDevice> mtlDevice = (__bridge id <MTLDevice>)(void*)data.device;
            id <MTLBinaryArchive> mtlArchive = [mtlDevice newBinaryArchiveWithDescriptor:[MTLBinaryArchiveDescriptor new]
                                                                                   error:&error];
            MTLRenderPipelineColorAttachmentDescriptor* mtlRenderPipelineColorAttachmentDescriptor = (__bridge MTLRenderPipelineColorAttachmentDescriptor*)(void*)data.blendState;
            MTLVertexDescriptor* mtlVertexDescriptor = (__bridge MTLVertexDescriptor*)(void*)data.vertexAttribute;
            id <MTLFunction> mtlMeshFunction = (__bridge id <MTLFunction>)(void*)data.meshShader;
            id <MTLFunction> mtlVertexFunction = (__bridge id <MTLFunction>)(void*)data.vertexShader;
            id <MTLFunction> mtlFragmentFunction = (__bridge id <MTLFunction>)(void*)data.fragmentShader;
            if (mtlMeshFunction)
            {
                if (@available(macOS 15, *))
                {
                    MTLMeshRenderPipelineDescriptor* mtlDesc = [MTLMeshRenderPipelineDescriptor new];
                    mtlDesc.meshFunction = mtlMeshFunction;
                    mtlDesc.fragmentFunction = mtlFragmentFunction;
                    mtlDesc.colorAttachments[0] = mtlRenderPipelineColorAttachmentDescriptor;
                    mtlDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
                    [mtlArchive addMeshRenderPipelineFunctionsWithDescriptor:mtlDesc
                                                                       error:&error];
                }
            }
            else
            {
                MTLRenderPipelineDescriptor* mtlDesc = [MTLRenderPipelineDescriptor new];
                mtlDesc.vertexDescriptor = mtlVertexDescriptor;
                mtlDesc.vertexFunction = mtlVertexFunction;
                mtlDesc.fragmentFunction = mtlFragmentFunction;
                mtlDesc.colorAttachments[0] = mtlRenderPipelineColorAttachmentDescriptor;
                mtlDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
                [mtlArchive addRenderPipelineFunctionsWithDescriptor:mtlDesc
                                                               error:&error];
            }
            if (error)
            {
                xxLog("Shader", "%s", error.localizedDescription.UTF8String);
            }
            [mtlArchive serializeToURL:[NSURL fileURLWithPath:@("/tmp/minamoto.pipeline.bin")]
                                 error:&error];
            if (error)
            {
                xxLog("Shader", "%s", error.localizedDescription.UTF8String);
            }

            // Disassembly
            xxFile* file = xxFile::Load("/tmp/minamoto.pipeline.bin");
            if (file)
            {
                size_t size = file->Size();
                std::vector<uint32_t> archive(size / sizeof(uint32_t));
                file->Read(archive.data(), size);
                delete file;
                remove("/tmp/minamoto.pipeline.bin");

                data.meshDisassembly.clear();
                data.vertexDisassembly.clear();
                data.fragmentDisassembly.clear();
                ShaderDisassemblerAGX::Disassemble(archive, [&](int found, int gpu, char const* type, void const* binary, size_t size)
                {
                    std::string& disassembly = [&]() -> std::string&
                    {
                        if (data.meshShader)
                        {
                            return (found == 1) ? data.fragmentDisassembly : data.meshDisassembly;
                        }
                        return (found == 1) ? data.vertexDisassembly : data.fragmentDisassembly;
                    }();

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
        }
    }
#endif
    if (strstr(xxGetInstanceName(), "OpenGL"))
    {
#if defined(xxMACOS)
        void* library = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
#elif defined(xxWINDOWS)
        void* library = xxLoadLibrary("opengl32.dll");
        void* (*wglGetProcAddress)(char const* name) = nullptr;
        (void*&)wglGetProcAddress = xxGetProcAddress(library, "wglGetProcAddress");
#else
        void* library = nullptr;
#endif
        int GL_SHADER_SOURCE_LENGTH = 0x8B88;
        void (*glGetShaderiv)(int shader, int pname, int* params) = nullptr;
        void (*glGetShaderSource)(int shader, int bufSize, int* length, char* source) = nullptr;
#if defined(xxWINDOWS)
        if (glGetShaderiv == nullptr && wglGetProcAddress)
            (void*&)glGetShaderiv = wglGetProcAddress("glGetShaderiv");
        if (glGetShaderSource == nullptr && wglGetProcAddress)
            (void*&)glGetShaderSource = wglGetProcAddress("glGetShaderSource");
#endif
        if (glGetShaderiv == nullptr)
            (void*&)glGetShaderiv = xxGetProcAddress(library, "glGetShaderiv");
        if (glGetShaderSource == nullptr)
            (void*&)glGetShaderSource = xxGetProcAddress(library, "glGetShaderSource");
        if (glGetShaderiv && glGetShaderSource)
        {
            int meshCount = 0;
            int vertexCount = 0;
            int fragmentCount = 0;
            glGetShaderiv(static_cast<int>(data.meshShader), GL_SHADER_SOURCE_LENGTH, &meshCount);
            glGetShaderiv(static_cast<int>(data.vertexShader), GL_SHADER_SOURCE_LENGTH, &vertexCount);
            glGetShaderiv(static_cast<int>(data.fragmentShader), GL_SHADER_SOURCE_LENGTH, &fragmentCount);
            data.meshDisassembly.resize(meshCount);
            data.vertexDisassembly.resize(vertexCount);
            data.fragmentDisassembly.resize(fragmentCount);
            glGetShaderSource(static_cast<int>(data.meshShader), meshCount, nullptr, data.meshDisassembly.data());
            glGetShaderSource(static_cast<int>(data.vertexShader), vertexCount, nullptr, data.vertexDisassembly.data());
            glGetShaderSource(static_cast<int>(data.fragmentShader), fragmentCount, nullptr, data.fragmentDisassembly.data());
        }
        xxFreeLibrary(library);
    }
    if (strstr(xxGetInstanceName(), "Vulkan"))
    {
#if defined(xxMACOS) || defined(xxIOS) || defined(xxWINDOWS)
        void (*Disassemble)(std::ostream& out, const std::vector<unsigned int>& stream) = nullptr;
#if defined(xxWINDOWS)
        void* library = xxLoadLibrary("glslang.dll");
#else
        void* library = xxLoadLibrary("libglslang.dylib");
#endif
        if (library)
        {
#if defined(xxWINDOWS)
            (void*&)Disassemble = xxGetProcAddress(library, "?Disassemble@spv@@YAXAEAV?$basic_ostream@DU?$char_traits@D@std@@@std@@AEBV?$vector@IV?$allocator@I@std@@@3@@Z");

            // Because the STL is not the same
#if defined(_LIBCPP_VERSION)
            Disassemble = nullptr;
#endif
#else
            (void*&)Disassemble = xxGetProcAddress(library, "_ZN3spv11DisassembleERNSt3__113basic_ostreamIcNS0_11char_traitsIcEEEERKNS0_6vectorIjNS0_9allocatorIjEEEE");
#endif
        }

        auto run = [&](uint64_t shader, std::string& disassembly)
        {
            disassembly.clear();
            if (shader == 0)
                return;
#if defined(xxWINDOWS)
            std::vector<uint32_t> spirv;
#else
            auto& spirv = *(std::vector<uint32_t>*)(shader + 96);
#endif
            if (Disassemble)
            {
                std::ostringstream stream;
                Disassemble(stream, spirv);
                disassembly = stream.str();
                return;
            }
            for (size_t i = 0; i < spirv.size(); ++i)
            {
                char line[64];
                snprintf(line, 64, "%zd : %08X\n", i, spirv[i]);
                disassembly += line;
            }
        };
        run(data.meshShader, data.meshDisassembly);
        run(data.vertexShader, data.vertexDisassembly);
        run(data.fragmentShader, data.fragmentDisassembly);
#endif
    }
}
//==============================================================================
void ShaderDisassembler::Initialize()
{
    if (xxCreatePipelineSystem)
        return;
    xxCreatePipelineSystem = xxCreatePipeline;
    xxCreatePipeline = xxCreatePipelineRuntime;
}
//------------------------------------------------------------------------------
void ShaderDisassembler::Shutdown()
{
    if (xxCreatePipelineSystem == nullptr)
        return;
    allShaderDisassembly.clear();
    xxCreatePipeline = xxCreatePipelineSystem;
    xxCreatePipelineSystem = nullptr;
}
//------------------------------------------------------------------------------
bool ShaderDisassembler::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    ImGui::SetNextWindowSize(ImVec2(1200.0f, 720.0f), ImGuiCond_Appearing);
    if (ImGui::Begin(ICON_FA_FILE_TEXT "Shader Disassembler", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        float width = (ImGui::GetWindowSize().x - 256.0f) / 2.0f;

        ImVec2 size = ImGui::GetWindowSize();
        size.x = size.x - ImGui::GetStyle().FramePadding.x * 8.0f;
        size.y = size.y - ImGui::GetCursorPosY() - (ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 6.0f);

        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 256.0f);
        ImGui::SetColumnWidth(1, width);
        ImGui::SetColumnWidth(2, width);

        // Architecture
        static int archCurrent = 0;
        static char const* const archList[] =
        {
#if defined(xxWINDOWS)
            "Direct3D",
#elif defined(xxMACOS) || defined(xxIOS)
            "Apple Graphics",
#endif
        };
        ImGui::SetNextItemWidth(128.0f);
        ImGui::Combo("Architecture", &archCurrent, archList, xxCountOf(archList));

        // Shader
        static int shaderCurrent = 0;
        ImGui::SetNextItemWidth(128.0f);
        ImGui::ListBox("Shader", &shaderCurrent, [](void*, int index) -> char const*
        {
            static char temp[64];
            auto it = allShaderDisassembly.begin();
            for (int i = 0; i < index && size_t(i) < allShaderDisassembly.size(); ++i)
                it++;
            auto& data = *(it);
            snprintf(temp, 64, "%016llX", data.first);
            return temp;
        }, nullptr, int(allShaderDisassembly.size()), int(size.y / ImGui::GetTextLineHeightWithSpacing()));

        auto it = allShaderDisassembly.begin();
        for (int i = 0; i < shaderCurrent && size_t(i) < allShaderDisassembly.size(); ++i)
            it++;
        auto& data = *(it);
        Disassemble(data.second);

        auto tabWindow = [&](float width, int flag)
        {
            if (data.second.meshShader && ImGui::BeginTabItem("Mesh", nullptr, flag))
            {
                ImGui::InputTextMultiline("", data.second.meshDisassembly.data(), data.second.meshDisassembly.size(), ImVec2(width, size.y), ImGuiInputTextFlags_ReadOnly);
                ImGui::EndTabItem();
            }
            if (data.second.vertexShader && ImGui::BeginTabItem("Vertex", nullptr, flag))
            {
                ImGui::InputTextMultiline("", data.second.vertexDisassembly.data(), data.second.vertexDisassembly.size(), ImVec2(width, size.y), ImGuiInputTextFlags_ReadOnly);
                ImGui::EndTabItem();
            }
            if (data.second.fragmentShader && ImGui::BeginTabItem("Fragment", nullptr, flag))
            {
                ImGui::InputTextMultiline("", data.second.fragmentDisassembly.data(), data.second.fragmentDisassembly.size(), ImVec2(width, size.y), ImGuiInputTextFlags_ReadOnly);
                ImGui::EndTabItem();
            }
        };

        ImGui::NextColumn();

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
}
//==============================================================================
