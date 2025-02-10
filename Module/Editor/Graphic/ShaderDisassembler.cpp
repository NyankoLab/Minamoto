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
                MTLMeshRenderPipelineDescriptor* mtlDesc = [MTLMeshRenderPipelineDescriptor new];
                mtlDesc.meshFunction = mtlMeshFunction;
                mtlDesc.fragmentFunction = mtlFragmentFunction;
                mtlDesc.colorAttachments[0] = mtlRenderPipelineColorAttachmentDescriptor;
                mtlDesc.depthAttachmentPixelFormat = MTLPixelFormatDepth32Float;
                [mtlArchive addMeshRenderPipelineFunctionsWithDescriptor:mtlDesc
                                                                   error:&error];
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
            char command[1024];
            xxFile* file = xxFile::Load("/tmp/minamoto.pipeline.bin");
            if (file)
            {
                size_t size = file->Size();
                std::vector<uint32_t> archive(size / sizeof(uint32_t));
                file->Read(archive.data(), size);
                delete file;

                // Archive
                int found = 0;
                for (size_t i = 0; i < size / sizeof(uint32_t); ++i)
                {
                    uint32_t magic = archive[i];
                    if (magic != 0xFEEDFACF)
                        continue;

                    if (found == 0)
                    {
                        found++;
                        
                        continue;
                    }

                    uint32_t size = archive[i + 0x90 / sizeof(uint32_t)];
                    uint32_t offset = archive[i + 0x98 / sizeof(uint32_t)];
                    size_t offset_prolog = i * sizeof(uint32_t) + offset;
                    size_t offset_main = SIZE_T_MAX;
                    for (size_t i = offset_prolog / sizeof(uint32_t); i < (offset_prolog + size) / sizeof(uint32_t); ++i)
                    {
                        uint32_t code = archive[i];
                        if (code == 'HASH')
                            break;
                        if (code == 0x00080008)
                        {
                            for (size_t j = i; j < (offset_prolog + size) / sizeof(uint32_t); ++j)
                            {
                                code = archive[j];
                                if (code == 'HASH')
                                    break;
                                if (code != 0x00080008)
                                {
                                    offset_main = j * sizeof(uint32_t);
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    if (found == 1 || found == 2)
                    {
                        found++;

                        snprintf(command, 1024, "/usr/bin/python3 %s/applegpu/disassemble.py /tmp/minamoto.pipeline.bin %zd > /tmp/minamoto.pipeline.txt", xxGetDocumentPath(), offset_prolog);
                        system(command);
                        if (offset_main != SIZE_T_MAX)
                        {
                            snprintf(command, 1024, "echo -------------------------------------------- >> /tmp/minamoto.vertex.txt");
                            system(command);
                            snprintf(command, 1024, "/usr/bin/python3 %s/applegpu/disassemble.py /tmp/minamoto.pipeline.bin %zd >> /tmp/minamoto.pipeline.txt", xxGetDocumentPath(), offset_main);
                            system(command);
                        }
                        file = xxFile::Load("/tmp/minamoto.pipeline.txt");
                        if (file)
                        {
                            size_t size = file->Size();
                            if (mtlMeshFunction)
                            {
                                data.meshDisassembly.resize(size);
                                file->Read(data.meshDisassembly.data(), size);
                            }
                            else if (found == 2)
                            {
                                data.vertexDisassembly.resize(size);
                                file->Read(data.vertexDisassembly.data(), size);
                            }
                            else
                            {
                                data.fragmentDisassembly.resize(size);
                                file->Read(data.fragmentDisassembly.data(), size);
                            }
                            delete file;
                        }
                        remove("/tmp/minamoto.pipeline.txt");
                        continue;
                    }
                }
            }
            remove("/tmp/minamoto.pipeline.bin");
        }
    }
#endif
    if (strstr(xxGetInstanceName(), "OpenGL"))
    {
#if defined(xxMACOS)
        void* library = xxLoadLibrary("/System/Library/Frameworks/OpenGL.framework/OpenGL");
#else
        void* library = nullptr;
#endif
        int GL_SHADER_SOURCE_LENGTH = 0x8B88;
        void (*glGetShaderiv)(int shader, int pname, int* params);
        void (*glGetShaderSource)(int shader, int bufSize, int* length, char* source);
        (void*&)glGetShaderiv = xxGetProcAddress(library, "glGetShaderiv");
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
#if defined(xxMACOS)
        void (*Disassemble)(std::ostream& out, const std::vector<unsigned int>& stream);
        void* library = xxLoadLibrary("libglslang.dylib");
        if (library)
        {
            (void*&)Disassemble = xxGetProcAddress(library, "_ZN3spv11DisassembleERNSt3__113basic_ostreamIcNS0_11char_traitsIcEEEERKNS0_6vectorIjNS0_9allocatorIjEEEE");
        }

        auto run = [&](uint64_t shader, std::string& disassembly)
        {
            disassembly.clear();
            if (shader == 0)
                return;
            auto& spirv = *(std::vector<uint32_t>*)(shader + 96);
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

    ImGui::SetNextWindowSize(ImVec2(1792.0f, 900.0f), ImGuiCond_Appearing);
    if (ImGui::Begin(ICON_FA_FILE_TEXT "Shader Disassembler", &show, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
    {
        ImVec2 size = ImGui::GetWindowSize();
        size.x = size.x - ImGui::GetStyle().FramePadding.x * 8.0f;
        size.y = size.y - ImGui::GetCursorPosY() - (ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 6.0f);

        ImGui::Columns(3);

        ImGui::SetColumnWidth(0, 256.0f);
        ImGui::SetColumnWidth(1, 768.0f);
        ImGui::SetColumnWidth(2, 768.0f);

        // Architecture
        static int archCurrent = 0;
        static char const* const archList[] =
        {
#if defined(xxWINDOWS)
            "Direct3D",
#elif defined(xxMACOS) || defined(xxIOS)
            "Apple G13G",
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
            for (int i = 0; i < index; ++i)
                it++;
            auto& data = *(it);
            snprintf(temp, 64, "%016llX", data.first);
            return temp;
        }, nullptr, int(allShaderDisassembly.size()), int(size.y / ImGui::GetTextLineHeightWithSpacing()));

        auto it = allShaderDisassembly.begin();
        for (int i = 0; i < shaderCurrent; ++i)
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
        tabWindow(768.0f, 0);
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
        tabWindow(768.0f, flag);
        ImGui::EndTabBar();

        ImGui::Columns(1);
    }
    ImGui::End();

    return false;
}
//==============================================================================
