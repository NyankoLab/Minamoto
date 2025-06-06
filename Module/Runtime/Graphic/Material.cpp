//==============================================================================
// Minamoto : Material Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include <xxGraphic/internal/xxGraphicInternal.h>
#include <xxGraphicPlus/xxTexture.h>
#include "Camera.h"
#include "Mesh.h"
#include "Node.h"
#include "Material.h"

//==============================================================================
//  MaterialSelector
//==============================================================================
struct MaterialSelector
{
    enum Language
    {
        GLSL,
        HLSL,
        HLSL10,
        HLSLVK,
        MSL1,
        MSL2
    };

    std::string& shader;
    Language language;
    int type;
    int tab;

    MaterialSelector(std::string& s, Language l, int t)
        : shader(s), language(l), type(t)
    {
        tab = 0;
    }
    void Append(std::string_view string)
    {
        if (string.empty() == false)
        {
            tab -= (string[0] == '}' || string[0] == ')') ? 1 : 0;
            if (string[0] != '#')
            {
                shader.append(4 * tab, ' ');
            }
            shader += string;
            shader += '\n';
            tab += (string[0] == '{' || string[0] == '(') ? 1 : 0;
        }
    }
    void Define(std::string_view name, std::string_view def, bool check = false)
    {
        if (check)
        {
            shader += "#ifndef";
            shader += ' ';
            shader += name;
            shader += '\n';
        }
        shader += "#define";
        shader += ' ';
        shader += name;
        shader += ' ';
        shader += def;
        shader += '\n';
        if (check)
        {
            shader += "#endif";
            shader += '\n';
        }
    }
    void Define(std::string_view name, int number)
    {
        Define(name, std::to_string(number));
    }
    void operator () (bool available, std::string_view string)
    {
        if (available == false)      return;
        Append(string);
    }
    void GM(bool available, std::string_view glsl, std::string_view msl)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = glsl; }
        else if (language == HLSL)   { string = glsl; }
        else if (language == HLSL10) { string = glsl; }
        else if (language == HLSLVK) { string = glsl; }
        else if (language == MSL1)   { string = msl;  }
        else if (language == MSL2)   { string = msl;  }
        Append(string);
    }
    void GH(bool available, std::string_view glsl, std::string_view hlsl)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = glsl; }
        else if (language == HLSL)   { string = hlsl; }
        else if (language == HLSL10) { string = hlsl; }
        else if (language == HLSLVK) { string = hlsl; }
        else if (language == MSL1)   { string = hlsl; }
        else if (language == MSL2)   { string = hlsl; }
        Append(string);
    }
    void GHM(bool available, std::string_view glsl, std::string_view hlsl, std::string_view msl)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = glsl; }
        else if (language == HLSL)   { string = hlsl; }
        else if (language == HLSL10) { string = hlsl; }
        else if (language == HLSLVK) { string = hlsl; }
        else if (language == MSL1)   { string = msl;  }
        else if (language == MSL2)   { string = msl;  }
        Append(string);
    }
    void GHHM(bool available, std::string_view glsl, std::string_view hlsl, std::string_view hlsl10, std::string_view msl)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = glsl;   }
        else if (language == HLSL)   { string = hlsl;   }
        else if (language == HLSL10) { string = hlsl10; }
        else if (language == HLSLVK) { string = hlsl10; }
        else if (language == MSL1)   { string = msl;    }
        else if (language == MSL2)   { string = msl;    }
        Append(string);
    }
    void GHMM(bool available, std::string_view glsl, std::string_view hlsl, std::string_view msl1, std::string_view msl2)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = glsl; }
        else if (language == HLSL)   { string = hlsl; }
        else if (language == HLSL10) { string = hlsl; }
        else if (language == HLSLVK) { string = hlsl; }
        else if (language == MSL1)   { string = msl1; }
        else if (language == MSL2)   { string = msl2; }
        Append(string);
    }
    void HM(bool available, std::string_view hlsl, std::string_view msl)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = hlsl; }
        else if (language == HLSL)   { string = hlsl; }
        else if (language == HLSL10) { string = hlsl; }
        else if (language == HLSLVK) { string = hlsl; }
        else if (language == MSL1)   { string = msl;  }
        else if (language == MSL2)   { string = msl;  }
        Append(string);
    }
    void HMM(bool available, std::string_view hlsl, std::string_view msl1, std::string_view msl2)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = hlsl; }
        else if (language == HLSL)   { string = hlsl; }
        else if (language == HLSL10) { string = hlsl; }
        else if (language == HLSLVK) { string = hlsl; }
        else if (language == MSL1)   { string = msl1; }
        else if (language == MSL2)   { string = msl2; }
        Append(string);
    }
    void HHMM(bool available, std::string_view hlsl, std::string_view hlsl10, std::string_view msl1, std::string_view msl2)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = hlsl;   }
        else if (language == HLSL)   { string = hlsl;   }
        else if (language == HLSL10) { string = hlsl10; }
        else if (language == HLSLVK) { string = hlsl10; }
        else if (language == MSL1)   { string = msl1;   }
        else if (language == MSL2)   { string = msl2;   }
        Append(string);
    }
    void HHHMM(bool available, std::string_view hlsl, std::string_view hlsl10, std::string_view hlslvk, std::string_view msl1, std::string_view msl2)
    {
        std::string_view string;
        if (available == false)      return;
        else if (language == GLSL)   { string = hlsl;   }
        else if (language == HLSL)   { string = hlsl;   }
        else if (language == HLSL10) { string = hlsl10; }
        else if (language == HLSLVK) { string = hlslvk; }
        else if (language == MSL1)   { string = msl1;   }
        else if (language == MSL2)   { string = msl2;   }
        Append(string);
    }
};
//==============================================================================
//  Material
//==============================================================================
xxMaterialPtr Material::DefaultMaterial;
//------------------------------------------------------------------------------
void Material::Setup(xxDrawData const& data)
{
    auto* node = data.node;
    if (node->ConstantDatas.size() <= data.materialIndex)
        node->ConstantDatas.resize(data.materialIndex + 1);
    auto* constantData = data.constantData = &node->ConstantDatas[data.materialIndex];

    if (constantData->ready == 0)
    {
        m_device = data.device;
        if (constantData->pipeline == 0)
            CreatePipeline(data);
        CreateConstant(data);
        constantData->ready = (constantData->pipeline != 0) ? 1 : -1;
    }

    UpdateConstant(data);
}
//------------------------------------------------------------------------------
void Material::Draw(xxDrawData const& data) const
{
    auto* constantData = data.constantData;

    xxSetPipeline(data.commandEncoder, constantData->pipeline);
    if (constantData->meshConstant)
    {
        xxSetMeshConstantBuffer(data.commandEncoder, constantData->meshConstant, constantData->meshConstantSize);
    }
    if (constantData->vertexConstant)
    {
        xxSetVertexConstantBuffer(data.commandEncoder, constantData->vertexConstant, constantData->vertexConstantSize);
    }
    if (constantData->fragmentConstant)
    {
        xxSetFragmentConstantBuffer(data.commandEncoder, constantData->fragmentConstant, constantData->fragmentConstantSize);
    }

    int textureCount = 0;
    uint64_t textures[16];
    uint64_t samplers[16];

    size_t count = std::min<size_t>(16, Textures.size());
    unsigned int slot = constantData->fragmentTextureSlot;
    for (unsigned int i = 0; i < count; ++i)
    {
        if (slot & 1)
        {
            xxTexturePtr const& texture = Textures[i];
            texture->Update(data.device);
            textures[i] = texture->Texture;
            samplers[i] = texture->Sampler;
            textureCount = i + 1;
        }
        else
        {
            textures[i] = 0;
            samplers[i] = 0;
        }
        slot >>= 1;
        if (slot == 0)
            break;
    }

    xxSetFragmentTextures(data.commandEncoder, textureCount, textures);
    xxSetFragmentSamplers(data.commandEncoder, textureCount, samplers);
}
//------------------------------------------------------------------------------
void Material::CreatePipeline(xxDrawData const& data)
{
    auto* mesh = data.mesh;
    uint64_t vertexAttribute = mesh->GetVertexAttribute();
    if (vertexAttribute == 0)
        return;

    auto* constantData = data.constantData;
    if (constantData->pipeline == 0)
    {
        if (m_blendState == 0)
        {
            if (Blending)
            {
                m_blendState = xxCreateBlendState(m_device, BlendSourceColor.c_str(),
                                                            BlendOperationColor.c_str(),
                                                            BlendDestinationColor.c_str(),
                                                            BlendSourceAlpha.c_str(),
                                                            BlendOperationAlpha.c_str(),
                                                            BlendDestinationAlpha.c_str());
            }
            else
            {
                m_blendState = xxCreateBlendState(m_device, "1", "+", "0", "1", "+", "0");
            }
        }
        if (m_depthStencilState == 0)
        {
            m_depthStencilState = xxCreateDepthStencilState(m_device, DepthTest.c_str(), DepthWrite);
        }
        if (m_rasterizerState == 0)
        {
            m_rasterizerState = xxCreateRasterizerState(m_device, Cull, (DebugWireframe == false), Scissor);
        }
        if (constantData->meshShader == 0 && constantData->vertexShader == 0 && constantData->fragmentShader == 0)
        {
            if (constantData->meshShader == 0 && mesh->Count[xxMesh::STORAGE0])
            {
                constantData->meshShader = xxCreateMeshShader(m_device, GetShader(data, 'mesh').c_str());
            }
            if (constantData->meshShader == 0 && constantData->vertexShader == 0)
            {
                constantData->vertexShader = xxCreateVertexShader(m_device, GetShader(data, 'vert').c_str(), vertexAttribute);
            }
            if (constantData->fragmentShader == 0)
            {
                constantData->fragmentShader = xxCreateFragmentShader(m_device, GetShader(data, 'frag').c_str());
            }
        }
        if (m_renderPass == 0)
        {
            m_renderPass = xxCreateRenderPass(m_device, true, true, true, true, true, true);
        }
        if (data.materialIndex == SELECT)
        {
            uint64_t blendState = xxCreateBlendState(m_device, "1", "+", "1", "1", "+", "0");
            uint64_t rasterizerState = xxCreateRasterizerState(m_device, Cull, (DebugWireframe == false), Scissor);
            constantData->pipeline = xxCreatePipeline(m_device, m_renderPass, blendState, m_depthStencilState, rasterizerState, vertexAttribute, constantData->meshShader, constantData->vertexShader, constantData->fragmentShader);
            xxDestroyBlendState(blendState);
            xxDestroyRasterizerState(rasterizerState);
        }
        else
        {
            constantData->pipeline = xxCreatePipeline(m_device, m_renderPass, m_blendState, m_depthStencilState, m_rasterizerState, vertexAttribute, constantData->meshShader, constantData->vertexShader, constantData->fragmentShader);
        }
    }
}
//------------------------------------------------------------------------------
void Material::CreateConstant(xxDrawData const& data) const
{
    auto* constantData = data.constantData;

    if (constantData->meshConstant == 0 && constantData->vertexConstant == 0 && constantData->fragmentConstant == 0)
    {
        constantData->device = data.device;
        if (constantData->meshShader && constantData->meshConstant == 0)
        {
            constantData->meshConstantSize = GetMeshConstantSize(data);
            if (constantData->meshConstantSize > 0)
            {
                constantData->meshConstant = xxCreateConstantBuffer(m_device, constantData->meshConstantSize);
            }
        }
        if (constantData->vertexShader && constantData->vertexConstant == 0)
        {
            constantData->vertexConstantSize = GetVertexConstantSize(data);
            if (constantData->vertexConstantSize > 0)
            {
                constantData->vertexConstant = xxCreateConstantBuffer(m_device, constantData->vertexConstantSize);
            }
        }
        if (constantData->fragmentShader && constantData->fragmentConstant == 0)
        {
            constantData->fragmentConstantSize = GetFragmentConstantSize(data);
            if (constantData->fragmentConstantSize > 0)
            {
                constantData->fragmentConstant = xxCreateConstantBuffer(m_device, constantData->fragmentConstantSize);
            }
        }
    }
}
//------------------------------------------------------------------------------
void Material::UpdateConstant(xxDrawData const& data) const
{
    int size;
    auto* constantData = data.constantData;

    size = constantData->meshConstantSize;
    if (size == 0)
        size = constantData->vertexConstantSize;
    if (size)
    {
        uint64_t constant = constantData->meshConstant;
        if (constant == 0)
            constant = constantData->vertexConstant;
        xxVector4* vector = reinterpret_cast<xxVector4*>(xxMapBuffer(m_device, constant));
        if (vector)
        {
            UpdateWorldViewProjectionConstant(data, size, &vector);
            if (constantData->meshConstant)
                UpdateCullingConstant(data, size, &vector);
            UpdateSkinningConstant(data, size, &vector);
            UpdateTransformConstant(data, size, &vector);
            UpdateBlendingConstant(data, size, &vector);
            UpdateLightingConstant(data, size, &vector);
            xxUnmapBuffer(m_device, constant);
        }
    }

    size = constantData->fragmentConstantSize;
    if (size)
    {
        uint64_t constant = constantData->fragmentConstant;
        xxVector4* vector = reinterpret_cast<xxVector4*>(xxMapBuffer(m_device, constant));
        if (vector)
        {
            UpdateAlphaTestingConstant(data, size, &vector);
            UpdateLightingConstant(data, size, &vector);
            xxUnmapBuffer(m_device, constant);
        }
    }
}
//------------------------------------------------------------------------------
std::string Material::GetShader(xxDrawData const& data, int type) const
{
    auto* mesh = data.mesh;
    auto* constantData = data.constantData;

    char const* deviceString = xxGetInstanceName();
    MaterialSelector::Language language = MaterialSelector::GLSL;
    if (language == 0 && strstr(deviceString, "Metal 2"))    language = MaterialSelector::MSL2;
    if (language == 0 && strstr(deviceString, "Metal"))      language = MaterialSelector::MSL1;
    if (language == 0 && strstr(deviceString, "Direct3D 1")) language = MaterialSelector::HLSL10;
    if (language == 0 && strstr(deviceString, "Direct3D"))   language = MaterialSelector::HLSL;
    if (language == 0 && strstr(deviceString, "Vulkan"))     language = MaterialSelector::HLSLVK;
    if (language == 0 && strstr(deviceString, "GL"))         language = MaterialSelector::GLSL;

    std::string shader;
    int meshTextureSlot = 0;
    int vertexTextureSlot = 0;
    int fragmentTextureSlot = 0;

    struct MaterialSelector s(shader, language, type);

    shader += ShaderOption;
    s.Define("DEBUG_NORMAL", DebugNormal ? 1 : 0);
    s.Define("SHADER_NORMAL", mesh->NormalCount);
    s.Define("SHADER_COLOR", mesh->ColorCount);
    s.Define("SHADER_TEXTURE", mesh->TextureCount);
    s.Define("SHADER_LIGHTING", mesh->NormalCount && Lighting ? 1 : 0);
    s.Define("SHADER_SPECULAR", mesh->NormalCount && Lighting && Specular ? 1 : 0);
    s.Define("TEXTURE_BASE", mesh->TextureCount && GetTexture(BASE) ? 1 : 0);
    s.Define("TEXTURE_BUMP", mesh->TextureCount && GetTexture(BUMP) ? 1 : 0);

    switch (type)
    {
    case 'mesh':
        s.Define("SHADER_UNIFORM", GetMeshConstantSize(data) / sizeof(xxVector4));
        s.Define("SHADER_BACKFACE_CULLING", BackfaceCulling ? 1 : 0);
        s.Define("SHADER_FRUSTUM_CULLING", FrustumCulling ? 1 : 0);
        s.Define("SHADER_OPACITY", Blending ? 1 : 0);
        ShaderDefault(data, s);
        ShaderAttribute(data, s);
        ShaderConstant(data, s);
        ShaderVarying(data, s);
        ShaderMesh(data, s);
        constantData->meshTextureSlot = meshTextureSlot;
        break;
    case 'vert':
        s.Define("SHADER_UNIFORM", GetVertexConstantSize(data) / sizeof(xxVector4));
        s.Define("SHADER_SKINNING", mesh->Skinning ? 1 : 0);
        s.Define("SHADER_OPACITY", Blending ? 1 : 0);
        ShaderDefault(data, s);
        ShaderAttribute(data, s);
        ShaderConstant(data, s);
        ShaderVarying(data, s);
        ShaderVertex(data, s);
        constantData->vertexTextureSlot = vertexTextureSlot;
        break;
    case 'frag':
        s.Define("SHADER_UNIFORM", GetFragmentConstantSize(data) / sizeof(xxVector4));
        s.Define("SHADER_ALPHATEST", AlphaTest ? 1 : 0);
        ShaderDefault(data, s);
        ShaderConstant(data, s);
        ShaderVarying(data, s);
        ShaderFragment(data, s);
        if (GetTexture(BASE))
        {
            fragmentTextureSlot |= 0b01;
        }
        if (GetTexture(BUMP))
        {
            fragmentTextureSlot |= 0b10;
        }
        constantData->fragmentTextureSlot = fragmentTextureSlot;
        break;
    }

    return shader;
}
//------------------------------------------------------------------------------
int Material::GetMeshConstantSize(xxDrawData const& data) const
{
    int size = 0;
    UpdateWorldViewProjectionConstant(data, size);
    UpdateCullingConstant(data, size);
    UpdateTransformConstant(data, size);
    UpdateBlendingConstant(data, size);
    UpdateLightingConstant(data, size);
    return size;
}
//------------------------------------------------------------------------------
int Material::GetVertexConstantSize(xxDrawData const& data) const
{
    int size = 0;
    UpdateWorldViewProjectionConstant(data, size);
    UpdateSkinningConstant(data, size);
    UpdateTransformConstant(data, size);
    UpdateBlendingConstant(data, size);
    UpdateLightingConstant(data, size);
    return size;
}
//------------------------------------------------------------------------------
int Material::GetFragmentConstantSize(xxDrawData const& data) const
{
    int size = 0;
    UpdateAlphaTestingConstant(data, size);
    UpdateLightingConstant(data, size);
    return size;
}
//------------------------------------------------------------------------------
void Material::ShaderDefault(xxDrawData const& data, struct MaterialSelector& s) const
{
    if (s.language == MaterialSelector::GLSL)
    {
        s.Define("float2", "vec2");
        s.Define("float3", "vec3");
        s.Define("float4", "vec4");
        s.Define("float2x2", "mat2");
        s.Define("float3x3", "mat3");
        s.Define("float4x4", "mat4");
        s.Define("int2", "ivec2");
        s.Define("int3", "ivec3");
        s.Define("int4", "ivec4");
    }
    if (s.language == MaterialSelector::GLSL ||
        s.language == MaterialSelector::MSL1 ||
        s.language == MaterialSelector::MSL2)
    {
        s.Define("mul(a, b)", "(a * b)");
    }
}
//------------------------------------------------------------------------------
void Material::ShaderAttribute(xxDrawData const& data, struct MaterialSelector& s) const
{
    auto* mesh = data.mesh;
    bool skinning = mesh->Skinning;
    int normal = mesh->NormalCount;
    int color = mesh->ColorCount;
    int texture = mesh->TextureCount;

    bool vertexPulling = s.type == 'mesh';
    if (vertexPulling)
    {
        //             GLSL / HLSL / MSL
        s(true,        "struct Attribute"     );
        s(true,        "{"                    );
        s(true,        "float Position[3];"   );
        s(skinning,    "float BoneWeight[3];" );
        s(skinning,    "uint BoneIndices[4];" );
        s(normal > 0,  "uint Normal;"         );
        s(normal > 1,  "uint Tangent;"        );
        s(normal > 2,  "uint Binormal;"       );
        s(color > 0,   "float Color[4];"      );
        s(texture > 0, "float UV0[2];"        );
        s(true,        "};"                   );
    }
    else
    {
        //                 GLSL                               HLSL                                 MSL
        s.GHM(true,        "",                                "struct Attribute",                  "struct Attribute"                              );
        s.GHM(true,        "",                                "{",                                 "{"                                             );
        s.GHM(true,        "attribute vec3 attrPosition;",    "float3 Position : POSITION;",       "float3 Position [[attribute(__COUNTER__)]];"   );
        s.GHM(skinning,    "attribute vec3 attrBoneWeight;",  "float3 BoneWeight : BLENDWEIGHT;",  "float3 BoneWeight [[attribute(__COUNTER__)]];" );
        s.GHM(skinning,    "attribute vec4 attrBoneIndices;", "uint4 BoneIndices : BLENDINDICES;", "uint4 BoneIndices [[attribute(__COUNTER__)]];" );
        s.GHM(normal > 0,  "attribute vec4 attrNormal;",      "uint4 Normal : NORMAL;",            "uint4 Normal [[attribute(__COUNTER__)]];"      );
        s.GHM(normal > 1,  "attribute vec4 attrTangent;",     "uint4 Tangent : TANGENT;",          "uint4 Tangent [[attribute(__COUNTER__)]];"     );
        s.GHM(normal > 2,  "attribute vec4 attrBinormal;",    "uint4 Binormal : BINORMAL;",        "uint4 Binormal [[attribute(__COUNTER__)]];"    );
        s.GHM(color > 0,   "attribute vec4 attrColor;",       "float4 Color : COLOR;",             "float4 Color [[attribute(__COUNTER__)]];"      );
        s.GHM(texture > 0, "attribute vec2 attrUV0;",         "float2 UV0 : TEXCOORD;",            "float2 UV0 [[attribute(__COUNTER__)]];"        );
        s.GHM(true,        "",                                "};",                                "};"                                            );
    }
}
//------------------------------------------------------------------------------
#define M xxStringify(MESH_UNIFORM)
#define V xxStringify(VERTEX_UNIFORM)
#define F xxStringify(FRAGMENT_UNIFORM)
#define T0 xxStringify(FRAGMENT_TEXTURE + 0)
#define T1 xxStringify(FRAGMENT_TEXTURE + 1)
#define S0 xxStringify(FRAGMENT_SAMPLER + 0)
#define S1 xxStringify(FRAGMENT_SAMPLER + 1)
//------------------------------------------------------------------------------
void Material::ShaderConstant(xxDrawData const& data, struct MaterialSelector& s) const
{
    bool mesh = s.type == 'mesh';
    bool vert = s.type == 'vert';
    bool frag = s.type == 'frag';
    bool base = s.type == 'frag' && GetTexture(BASE) != nullptr;
    bool bump = s.type == 'frag' && GetTexture(BUMP) != nullptr;

    if (mesh)
    {
        s(true, "struct Meshlet"         );
        s(true, "{"                      );
        s(true, "uint VertexOffset;"     );
        s(true, "uint TriangleOffset;"   );
        s(true, "uint VertexCount;"      );
        s(true, "uint TriangleCount;"    );
        s(true, "float4 CenterRadius;"   );
        s(true, "float4 ConeApex;"       );
        s(true, "float4 ConeAxisCutoff;" );
        s(true, "};"                     );
    }

    //            GLSL / HLSL                                  HLSL10                                   MSL                                           MSL Argument
    s.HHMM(true,  "",                                          "",                                      "",                                           "struct Uniform"                         );
    s.HHMM(true,  "",                                          "",                                      "",                                           "{"                                      );
    s.HHMM(mesh,  "",                                          "",                                      "struct MeshBuffer",                          ""                                       );
    s.HHMM(mesh,  "",                                          "",                                      "{",                                          ""                                       );
    s.HHMM(mesh,  "",                                          "",                                      "device Attribute* Vertices [[buffer(0)]];",  "device Attribute* Vertices [[id(0)]];"  );
    s.HHMM(mesh,  "",                                          "",                                      "device Meshlet* Meshlets [[buffer(1)]];",    "device Meshlet* Meshlets [[id(1)]];"    );
    s.HHMM(mesh,  "",                                          "",                                      "device uint* VertexIndices [[buffer(2)]];",  "device uint* VertexIndices [[id(2)]];"  );
    s.HHMM(mesh,  "",                                          "",                                      "device uint* TriangeIndices [[buffer(3)]];", "device uint* TriangeIndices [[id(3)]];" );
    s.HHMM(mesh,  "",                                          "",                                      "};",                                         ""                                       );
    s.HHHMM(mesh, "", "cbuffer Uniform",         "[[vk::binding(" M ", 0)]] cbuffer Uniform",           "struct Uniform",                             ""                                       );
    s.HHHMM(vert, "", "cbuffer Uniform",         "[[vk::binding(" V ", 0)]] cbuffer Uniform",           "struct Uniform",                             ""                                       );
    s.HHHMM(frag, "", "cbuffer Uniform",         "[[vk::binding(" F ", 0)]] cbuffer Uniform",           "struct Uniform",                             ""                                       );
    s.HHMM(true,  "",                                          "{",                                     "{",                                          ""                                       );
    s.HHMM(true,  "#if SHADER_UNIFORM",                        "#if SHADER_UNIFORM",                    "#if SHADER_UNIFORM",                         ""                                       );
    s.HHMM(mesh,  "uniform float4 uniBuffer[SHADER_UNIFORM];", "float4 uniBuffer[SHADER_UNIFORM];",     "float4 Buffer[SHADER_UNIFORM];",             "device float4* Buffer [[id(" M ")]];"   );
    s.HHMM(vert,  "uniform float4 uniBuffer[SHADER_UNIFORM];", "float4 uniBuffer[SHADER_UNIFORM];",     "float4 Buffer[SHADER_UNIFORM];",             "device float4* Buffer [[id(" V ")]];"   );
    s.HHMM(frag,  "uniform float4 uniBuffer[SHADER_UNIFORM];", "float4 uniBuffer[SHADER_UNIFORM];",     "float4 Buffer[SHADER_UNIFORM];",             "device float4* Buffer [[id(" F ")]];"   );
    s.HHMM(true,  "#endif",                                    "#endif",                                "#endif",                                     ""                                       );
    s.HHMM(true,  "",                                          "};",                                    "};",                                         ""                                       );
    s.HHMM(true,  "",                                          "",                                      "struct Sampler",                             ""                                       );
    s.HHMM(true,  "",                                          "",                                      "{",                                          ""                                       );
    s.HHHMM(base, "", "Texture2D<float4> Base;", "[[vk::binding(" T0 ", 0)]] Texture2D<float4> Base;",  "texture2d<float> Base [[texture(0)]];",      "texture2d<float> Base [[id(" T0 ")]];"  );
    s.HHHMM(bump, "", "Texture2D<float4> Bump;", "[[vk::binding(" T1 ", 0)]] Texture2D<float4> Bump;",  "texture2d<float> Bump [[texture(1)]];",      "texture2d<float> Bump [[id(" T1 ")]];"  );
    s.HHHMM(base, "", "sampler BaseSampler;",    "[[vk::binding(" S0 ", 0)]] sampler BaseSampler;",     "sampler BaseSampler [[sampler(0)]];",        "sampler BaseSampler [[id(" S0 ")]];"    );
    s.HHHMM(bump, "", "sampler BumpSampler;",    "[[vk::binding(" S1 ", 0)]] sampler BumpSampler;",     "sampler BumpSampler [[sampler(1)]];",        "sampler BumpSampler [[id(" S1 ")]];"    );
    s.HHMM(base,  "uniform sampler2D BaseSampler;",            "",                                      "",                                           ""                                       );
    s.HHMM(bump,  "uniform sampler2D BumpSampler;",            "",                                      "",                                           ""                                       );
    s.HHMM(true,  "",                                          "",                                      "};",                                         "};"                                     );
    s.HHMM(frag,  "",                                          "",                                      "",                                           "struct Sampler {};"                     );
}
//------------------------------------------------------------------------------
void Material::ShaderVarying(xxDrawData const& data, struct MaterialSelector& s) const
{
    auto* mesh = data.mesh;
    int normal = mesh->NormalCount;
    int color = mesh->ColorCount;
    int texture = mesh->TextureCount;
    int fragNormal = (Lighting || DebugNormal) ? normal : 0;

    //                          GLSL                               HLSL                                 MSL
    s.GHM(true,                 "",                                "struct Varying",                    "struct Varying"                );
    s.GHM(true,                 "",                                "{",                                 "{"                             );
    s.GHM(true,                 "",                                "float4 Position : SV_POSITION;",    "float4 Position [[position]];" );
    s.GHM(Lighting || color,    "varying vec4 varyColor;",         "float4 Color : COLOR;",             "float4 Color;"                 );
    s.GHM(texture > 0,          "varying vec2 varyUV0;",           "float2 UV0 : TEXCOORD0;",           "float2 UV0;"                   );
    s.GHM(Lighting && Specular, "varying vec3 varyWorldPosition;", "float3 WorldPosition : TEXCOORD4;", "float3 WorldPosition;"         );
    s.GHM(fragNormal > 0,       "varying vec3 varyWorldNormal;",   "float3 WorldNormal : TEXCOORD5;",   "float3 WorldNormal;"           );
    s.GHM(fragNormal > 1,       "varying vec3 varyWorldTangent;",  "float3 WorldTangent : TEXCOORD6;",  "float3 WorldTangent;"          );
    s.GHM(fragNormal > 2,       "varying vec3 varyWorldBinormal;", "float3 WorldBinormal : TEXCOORD7;", "float3 WorldBinormal;"         );
    s.GHM(true,                 "",                                "};",                                "};"                            );
}
//------------------------------------------------------------------------------
void Material::ShaderMesh(xxDrawData const& data, struct MaterialSelector& s) const
{
    auto* mesh = data.mesh;
    bool skinning = mesh->Skinning;
    int normal = mesh->NormalCount;
    int color = mesh->ColorCount;
    int texture = mesh->TextureCount;
    int fragNormal = (Lighting || DebugNormal) ? normal : 0;

    //             HLSL                                MSL                                                        MSL Arugment
    s.HMM(true,    "[outputtopology(\"triangle\")]",   "[[mesh]]",                                                "[[mesh]]"                                                );
    s.HMM(true,    "[numthreads(128, 1, 1)",           "",                                                        ""                                                        );
    s.HMM(true,    "void Main",                        "void Main",                                               "void Main"                                               );
    s.HMM(true,    "(",                                "(",                                                       "("                                                       );
    s.HMM(true,    "",                                 "constant Uniform& uni [[buffer(" M ")]], MeshBuffer mb,", "constant Uniform& uni [[buffer(" M ")]],"                );
    s.HMM(true,    "uint gtid : SV_GroupThreadID,",    "uint gtid [[thread_position_in_threadgroup]],",           "uint gtid [[thread_position_in_threadgroup]],"           );
    s.HMM(true,    "uint gid : SV_GroupID,",           "uint gid [[threadgroup_position_in_grid]],",              "uint gid [[threadgroup_position_in_grid]],"              );
    s.HMM(true,    "out indices uint3 triangles[64],", "mesh<Varying, void, 64, 128, topology::triangle> output", "mesh<Varying, void, 64, 128, topology::triangle> output" );
    s.HMM(true,    "out vertices Varying vary[128]",   "",                                                        ""                                                        );
    s.HMM(true,    ")",                                ")",                                                       ")"                                                       );
    s.HMM(true,    "{",                                "{",                                                       "{"                                                       );
    s.HMM(true,    "int uniIndex = 0;",                "int uniIndex = 0;",                                       "int uniIndex = 0;"                                       );
    s.HMM(true,    "",                                 "auto uniBuffer = uni.Buffer;",                            "auto uniBuffer = uni.Buffer;"                            );
    s.HMM(true,    "",                                 "device Attribute* Vertices = mb.Vertices;",               "device Attribute* Vertices = uni.Vertices;"              );
    s.HMM(true,    "",                                 "device uint* VertexIndices = mb.VertexIndices;",          "device uint* VertexIndices = uni.VertexIndices;"         );
    s.HMM(true,    "",                                 "device uint* TriangeIndices = mb.TriangeIndices;",        "device uint* TriangeIndices = uni.TriangeIndices;"       );
    s.HMM(true,    "Meshlet& m = Meshlets[gid];",      "device Meshlet& m = mb.Meshlets[gid];",                   "device Meshlet& m = uni.Meshlets[gid];"                  );

    int size = 0;
    UpdateWorldViewProjectionConstant(data, size, nullptr, &s);
    UpdateCullingConstant(data, size, nullptr, &s);

    s(true,         "if (gtid == 0)"                                                                                                     );
    s(true,         "{"                                                                                                                  );
    s.HM(true,      "SetMeshOutputCounts(m.VertexCount, m.TriangleCount);", ""                                                           );
    s.HM(true,  "", "output.set_primitive_count(m.TriangleCount);"                                                                       );
    s(true,         "}"                                                                                                                  );
    s(true,         "if (gtid < m.TriangleCount)"                                                                                        );
    s(true,         "{"                                                                                                                  );
    s(true,         "uint index = 3 * gtid;"                                                                                             );
    s(true,         "uint packed = TriangeIndices[m.TriangleOffset + gtid];"                                                             );
    s.HM(true,      "triangles[gtid].x = (packed >>  0) & 0xFF);", ""                                                                    );
    s.HM(true,      "triangles[gtid].y = (packed >>  8) & 0xFF);", ""                                                                    );
    s.HM(true,      "triangles[gtid].z = (packed >> 16) & 0xFF);", ""                                                                    );
    s.HM(true,  "", "output.set_index(index + 0, (packed >>  0) & 0xFF);"                                                                );
    s.HM(true,  "", "output.set_index(index + 1, (packed >>  8) & 0xFF);"                                                                );
    s.HM(true,  "", "output.set_index(index + 2, (packed >> 16) & 0xFF);"                                                                );
    s(true,         "}"                                                                                                                  );
    s(true,         "if (gtid >= m.VertexCount) return;"                                                                                 );
    s(true,         "uint vertexIndex = VertexIndices[m.VertexOffset + gtid];"                                                           );
    s(true,         "Attribute attr = Vertices[vertexIndex];"                                                                            );
    s(true,         "float3 attrPosition = float3(attr.Position[0], attr.Position[1], attr.Position[2]);"                                );
    s(skinning,     "float3 attrBoneWeight = float3(attr.BoneWeight[0], attr.BoneWeight[1], attr.BoneWeight[2]);"                        );
    s(skinning,     "uint4 attrBoneIndices = uint4(attr.BoneIndices[0], attr.BoneIndices[1], attr.BoneIndices[2], attr.BoneIndices[3]);" );
    s(color,        "float4 attrColor = float4(attr.Color[0], attr.Color[1], attr.Color[2], attr.Color[3]);"                             );
    s(texture > 0,  "float2 attrUV0 = float2(attr.UV0[0], attr.UV0[1]);"                                                                 );
    s(normal > 0,   "float3 attrNormal = float3(attr.Normal & 0xFF, (attr.Normal >> 8) & 0xFF, (attr.Normal >> 16) & 0xFF);"             );
    s(normal > 1,   "float3 attrTangent = float3(attr.Tangent & 0xFF, (attr.Tangent >> 8) & 0xFF, (attr.Tangent >> 16) & 0xFF);"         );
    s(normal > 2,   "float3 attrBinormal = float3(attr.Binormal & 0xFF, (attr.Binormal >> 8) & 0xFF, (attr.Binormal >> 16) & 0xFF);"     );
    s(true,         "float4 color = float4(1.0, 1.0, 1.0, 1.0);"                                                                         );
    s(normal > 0,   "float3 normal = attrNormal / 127.5 - 1.0;"                                                                          );
    s(normal > 1,   "float3 tangent = attrTangent / 127.5 - 1.0;"                                                                        );
    s(normal > 2,   "float3 binormal = attrBinormal / 127.5 - 1.0;"                                                                      );
    s(color,        "color = attrColor;"                                                                                                 );
    s(DebugMeshlet, "uint hash = gid * -16777619;"                                                                                       );
    s(DebugMeshlet, "color.rgb = float3(uint3(hash & 0xFF, (hash >> 8) & 0xFF, (hash >> 16) & 0xFF)) / 255.0;"                           );

    UpdateTransformConstant(data, size, nullptr, &s);
    UpdateBlendingConstant(data, size, nullptr, &s);
    UpdateLightingConstant(data, size, nullptr, &s);

    //                         HLSL                                             MSL
    s.HM(true,                 "",                                              "Varying vary;"                           );
    s.HM(true,                 "vary[gtid].Position = screenPosition;",         "vary.Position = screenPosition;"         );
    s.HM(Lighting || color,    "vary[gtid].Color = color;",                     "vary.Color = color;"                     );
    s.HM(texture > 0,          "vary[gtid].UV0 = attrUV0;",                     "vary.UV0 = attrUV0;"                     );
    s.HM(Lighting && Specular, "vary[gtid].WorldPosition = worldPosition.xyz;", "vary.WorldPosition = worldPosition.xyz;" );
    s.HM(fragNormal > 0,       "vary[gtid].WorldNormal = worldNormal;",         "vary.WorldNormal = worldNormal;"         );
    s.HM(fragNormal > 1,       "vary[gtid].WorldTangent = worldTangent;",       "vary.WorldTangent = worldTangent;"       );
    s.HM(fragNormal > 2,       "vary[gtid].WorldBinormal = worldBinormal;",     "vary.WorldBinormal = worldBinormal;"     );
    s.HM(true,                 "",                                              "output.set_vertex(gtid, vary);"          );
    s.HM(true,                 "}",                                             "}"                                       );
}
//------------------------------------------------------------------------------
void Material::ShaderVertex(xxDrawData const& data, struct MaterialSelector& s) const
{
    auto* mesh = data.mesh;
    bool skinning = mesh->Skinning;
    int normal = mesh->NormalCount;
    int color = mesh->ColorCount;
    int texture = mesh->TextureCount;
    int fragNormal = (Lighting || DebugNormal) ? normal : 0;

    //          GLSL           HLSL              MSL
    s.GHM(true, "",            "",               "vertex"                                  );
    s.GHM(true, "void main()", "Varying Main",   "Varying Main"                            );
    s.GHM(true, "",            "(",              "("                                       );
    s.GHM(true, "",            "Attribute attr", "Attribute attr [[stage_in]],"            );
    s.GHM(true, "",            "",               "constant Uniform& uni [[buffer(" V ")]]" );
    s.GHM(true, "",            ")",              ")"                                       );
    s.GHM(true, "{",           "{",              "{"                                       );
    s.GHM(true, "",            "",               "auto uniBuffer = uni.Buffer;"            );

    //                GLSL                       HLSL / MSL
    s.GH(true,        "int uniIndex = 0;",       "int uniIndex = 0;"                         );
    s.GH(true,        "vec4 color = vec4(1.0);", "float4 color = 1.0;"                       );
    s.GH(true,        "",                        "float3 attrPosition = attr.Position;"      );
    s.GH(skinning,    "",                        "float3 attrBoneWeight = attr.BoneWeight;"  );
    s.GH(skinning,    "",                        "uint4 attrBoneIndices = attr.BoneIndices;" );
    s.GH(color,       "",                        "float4 attrColor = attr.Color;"            );
    s.GH(texture > 0, "",                        "float2 attrUV0 = attr.UV0;"                );
    s.GH(normal > 0,  "",                        "uint4 attrNormal = attr.Normal;"           );
    s.GH(normal > 1,  "",                        "uint4 attrTangent = attr.Tangent;"         );
    s.GH(normal > 2,  "",                        "uint4 attrBinormal = attr.Binormal;"       );

    //            GLSL / HLSL / MSL
    s(normal > 0, "float3 normal = float3(attrNormal.x, attrNormal.y, attrNormal.z) / 127.5 - 1.0;"         );
    s(normal > 1, "float3 tangent = float3(attrTangent.x, attrTangent.y, attrTangent.z) / 127.5 - 1.0;"     );
    s(normal > 2, "float3 binormal = float3(attrBinormal.x, attrBinormal.y, attrBinormal.z) / 127.5 - 1.0;" );
    s(color,      "color = attrColor;"                                                                      );

    int size = 0;
    UpdateWorldViewProjectionConstant(data, size, nullptr, &s);
    UpdateSkinningConstant(data, size, nullptr, &s);
    UpdateTransformConstant(data, size, nullptr, &s);
    UpdateBlendingConstant(data, size, nullptr, &s);
    UpdateLightingConstant(data, size, nullptr, &s);

    //                         GLSL                                      HLSL / MSL
    s.GH(true,                 "",                                       "Varying vary;"                           );
    s.GH(true,                 "gl_Position = screenPosition;",          "vary.Position = screenPosition;"         );
    s.GH(Lighting || color,    "varyColor = color;",                     "vary.Color = color;"                     );
    s.GH(texture > 0,          "varyUV0 = attrUV0;",                     "vary.UV0 = attrUV0;"                     );
    s.GH(Lighting && Specular, "varyWorldPosition = worldPosition.xyz;", "vary.WorldPosition = worldPosition.xyz;" );
    s.GH(fragNormal > 0,       "varyWorldNormal = worldNormal;",         "vary.WorldNormal = worldNormal;"         );
    s.GH(fragNormal > 1,       "varyWorldTangent = worldTangent;",       "vary.WorldTangent = worldTangent;"       );
    s.GH(fragNormal > 2,       "varyWorldBinormal = worldBinormal;",     "vary.WorldBinormal = worldBinormal;"     );
    s.GH(true,                 "",                                       "return vary;"                            );
    s.GH(true,                 "}",                                      "}"                                       );
}
//------------------------------------------------------------------------------
void Material::ShaderFragment(xxDrawData const& data, struct MaterialSelector& s) const
{
    auto* mesh = data.mesh;
    int normal = mesh->NormalCount;
    int color = mesh->ColorCount;
    int texture = mesh->TextureCount;
    int fragNormal = (Lighting || DebugNormal) ? normal : 0;

    bool base = texture && GetTexture(BASE) != nullptr;
    bool bump = texture && GetTexture(BUMP) != nullptr;

    //          GLSL           HLSL            MSL
    s.GHM(true, "",            "",             "fragment"                                 );
    s.GHM(true, "void main()", "float4 Main",  "float4 Main"                              );
    s.GHM(true, "",            "(",            "("                                        );
    s.GHM(true, "",            "Varying vary", "Varying vary [[stage_in]],"               );
    s.GHM(true, "",            "",             "constant Uniform& uni [[buffer(" F ")]]," );
    s.GHM(true, "",            "",             "Sampler sam"                              );
    s.GHM(true, "",            ") : COLOR0",   ")"                                        );
    s.GHM(true, "{",           "{",            "{"                                        );
    s.GHM(true, "",            "",             "#if SHADER_UNIFORM"                       );
    s.GHM(true, "",            "",             "auto uniBuffer = uni.Buffer;"             );
    s.GHM(true, "",            "",             "#endif"                                   );

    //                         GLSL                       HLSL / MSL
    s.GH(true,                 "int uniIndex = 0;",       "int uniIndex = 0;"                              );
    s.GH(true,                 "vec4 color = vec4(1.0);", "float4 color = 1.0;"                            );
    s.GH(bump,                 "vec4 bump = vec4(0.0);",  "float4 bump = 0.0;"                             );
    s.GH(Lighting || color,    "",                        "float4 varyColor = vary.Color;"                 );
    s.GH(texture > 0,          "",                        "float2 varyUV0 = vary.UV0;"                     );
    s.GH(Lighting && Specular, "",                        "float3 varyWorldPosition = vary.WorldPosition;" );
    s.GH(fragNormal > 0,       "",                        "float3 varyWorldNormal = vary.WorldNormal;"     );
    s.GH(fragNormal > 1,       "",                        "float3 varyWorldTangent = vary.WorldTangent;"   );
    s.GH(fragNormal > 2,       "",                        "float3 varyWorldBinormal = vary.WorldBinormal;" );
    s.GH(Lighting || color,    "color = varyColor;",      "color = varyColor;"                             );

    //          GLSL / HLSL MSL                                    MSL Argument
    s.HMM(base, "",         "auto Base = sam.Base;",               "auto Base = uni.Base;"               );
    s.HMM(bump, "",         "auto Bump = sam.Bump;",               "auto Bump = uni.Bump;"               );
    s.HMM(base, "",         "auto BaseSampler = sam.BaseSampler;", "auto BaseSampler = uni.BaseSampler;" );
    s.HMM(bump, "",         "auto BumpSampler = sam.BumpSampler;", "auto BumpSampler = uni.BumpSampler;" );

    //           GLSL                                         HLSL                                     HLSL10                                         MSL
    s.GHHM(base, "color *= texture2D(BaseSampler, varyUV0);", "color *= tex2D(BaseSampler, varyUV0);", "color *= Base.Sample(BaseSampler, varyUV0);", "color *= Base.sample(BaseSampler, varyUV0);" );
    s.GHHM(bump, "bump = texture2D(BumpSampler, varyUV0);",   "bump = tex2D(BumpSampler, varyUV0);",   "bump = Bump.Sample(BumpSampler, varyUV0);",   "bump = Bump.sample(BumpSampler, varyUV0);"   );
    s.GHHM(bump, "bump = bump * 2.0 - 1.0;",                  "bump = bump * 2.0 - 1.0;",              "bump = bump * 2.0 - 1.0;",                    "bump = bump * 2.0 - 1.0;"                    );

    int size = 0;
    UpdateAlphaTestingConstant(data, size, nullptr, &s);
    UpdateLightingConstant(data, size, nullptr, &s);

    //         GLSL                     HLSL / MSL
    s.GH(true, "gl_FragColor = color;", "return color;" );
    s.GH(true, "}",                     "}"             );
}
//------------------------------------------------------------------------------
#undef M
#undef V
#undef F
#undef T0
#undef T1
#undef S0
#undef S1
//------------------------------------------------------------------------------
void Material::UpdateAlphaTestingConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (AlphaTest == false)
        return;
    if (pointer == nullptr)
    {
        size += 1 * sizeof(xxVector4);
    }
    if (size >= 1 * sizeof(xxVector4) && pointer)
    {
        xxVector4* vector = (*pointer);
        size -= 1 * sizeof(xxVector4);
        (*pointer) += 1;

        vector[0].x = AlphaTestReference;
    }
    if (s)
    {
        (*s)(true,     "float alphaRef = uniBuffer[uniIndex++].x;"    );
        (*s)(true,     "if (color.a < alphaRef)"                      );
        (*s)(true,     "{"                                            );
        (*s).GHM(true, "discard;", "clip(-1);", "discard_fragment();" );
        (*s)(true,     "}"                                            );
    }
}
//------------------------------------------------------------------------------
void Material::UpdateBlendingConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (Blending == false)
        return;
    if (pointer == nullptr)
    {
        size += 1 * sizeof(xxVector4);
    }
    if (size >= 1 * sizeof(xxVector4) && pointer)
    {
        xxVector4* vector = (*pointer);
        size -= 1 * sizeof(xxVector4);
        (*pointer) += 1;

        vector[0].x = Opacity;
    }
    if (s)
    {
        (*s)(true, "color.a = uniBuffer[uniIndex++].x;");
    }
}
//------------------------------------------------------------------------------
void Material::UpdateCullingConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (BackfaceCulling == false && FrustumCulling == false)
        return;
    if (pointer == nullptr)
    {
        size += 6 * sizeof(xxMatrix4x2);
    }
    if (size >= 6 * sizeof(xxMatrix4x2) && pointer)
    {
        xxMatrix4x2* frustum = reinterpret_cast<xxMatrix4x2*>(*pointer);
        size -= 6 * sizeof(xxMatrix4x2);
        (*pointer) += 6 * 2;

        if (data.frustum)
        {
            for (int i = 0; i < 6; ++i)
            {
                frustum[i] = data.frustum[i];
            }
        }
        else
        {
            for (int i = 0; i < 6; ++i)
            {
                frustum[i] = {};
            }
        }
    }
    if (s)
    {
        (*s)(true,            "uint visible = 1;"                                                                              );
        (*s).HM(true,         "if (WaveIsFirstLane())", ""                                                                     );
        (*s).HM(true,     "", "if (simd_is_first())"                                                                           );
        (*s)(true,            "{"                                                                                              );
        (*s)(BackfaceCulling, "if (visible)"                                                                                   );
        (*s)(BackfaceCulling, "{"                                                                                              );
        (*s)(BackfaceCulling, "float4 coneApex = m.ConeApex * world;"                                                          );
        (*s)(BackfaceCulling, "float4 coneAxisCutoff = m.ConeAxisCutoff * world;"                                              );
        (*s)(BackfaceCulling, "float c = m.ConeAxisCutoff.w;"                                                                  );
        (*s)(BackfaceCulling, "float d = dot(coneAxisCutoff.xyz, normalize(coneApex.xyz - uniBuffer[uniIndex + 1].xyz));"      );
        (*s)(BackfaceCulling, "if (d >= c) visible = 0;"                                                                       );
        (*s)(BackfaceCulling, "}"                                                                                              );
        (*s)(FrustumCulling,  "if (visible)"                                                                                   );
        (*s)(FrustumCulling,  "{"                                                                                              );
        (*s)(FrustumCulling,  "float4 centerRadius = m.CenterRadius * world;"                                                  );
        (*s)(FrustumCulling,  "float r = -m.CenterRadius.w;"                                                                   );
        (*s)(FrustumCulling,  "float d0 = dot(uniBuffer[uniIndex + 0].xyz, centerRadius.xyz - uniBuffer[uniIndex + 1].xyz);"   );
        (*s)(FrustumCulling,  "float d1 = dot(uniBuffer[uniIndex + 2].xyz, centerRadius.xyz - uniBuffer[uniIndex + 3].xyz);"   );
        (*s)(FrustumCulling,  "float d2 = dot(uniBuffer[uniIndex + 4].xyz, centerRadius.xyz - uniBuffer[uniIndex + 5].xyz);"   );
        (*s)(FrustumCulling,  "float d3 = dot(uniBuffer[uniIndex + 6].xyz, centerRadius.xyz - uniBuffer[uniIndex + 7].xyz);"   );
        (*s)(FrustumCulling,  "float d4 = dot(uniBuffer[uniIndex + 8].xyz, centerRadius.xyz - uniBuffer[uniIndex + 9].xyz);"   );
        (*s)(FrustumCulling,  "float d5 = dot(uniBuffer[uniIndex + 10].xyz, centerRadius.xyz - uniBuffer[uniIndex + 11].xyz);" );
        (*s)(FrustumCulling,  "if (d0 < r || d1 < r || d2 < r || d3 < r || d4 < r || d5 < r) visible = 0;"                     );
        (*s)(FrustumCulling,  "}"                                                                                              );
        (*s)(true,            "}"                                                                                              );
        (*s).HM(true,         "visible = WaveReadLaneFirst(visible);", ""                                                      );
        (*s).HM(true,     "", "visible = simd_broadcast_first(visible);"                                                       );
        (*s)(true,            "if (visible == 0) return;"                                                                      );
        (*s)(true,            "uniIndex += 12;"                                                                                );
    }
}
//------------------------------------------------------------------------------
void Material::UpdateLightingConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (Lighting == false && DebugNormal == false)
        return;
    if (pointer == nullptr)
    {
        size += 7 * sizeof(xxVector4);
    }
    if (size >= 7 * sizeof(xxVector4) && pointer)
    {
        xxVector4* vector = (*pointer);
        size -= 7 * sizeof(xxVector4);
        (*pointer) += 7;

        Camera* camera = (Camera*)data.camera;
        if (camera)
        {
            vector[0].xyz = camera->Location;
            vector[1].w = camera->LambertStep;
            vector[1].xyz = camera->LightDirection;
            vector[2].xyz = camera->LightColor;
        }
        else
        {
            vector[0].xyz = xxVector3::ZERO;
            vector[1].w = 0.5f;
            vector[1].xyz = xxVector3::Y;
            vector[2].xyz = xxVector3::WHITE;
        }
        vector[3].xyz = AmbientColor;
        vector[4].xyz = DiffuseColor;
        vector[5].xyz = EmissiveColor;
        vector[6].xyz = SpecularColor;
        vector[6].w = SpecularHighlight;
    }
    if (s)
    {
        int normal = data.mesh->NormalCount;

        bool mesh = s->type == 'mesh';
        bool vert = s->type == 'vert';
        bool frag = s->type == 'frag';

        bool bump = GetTexture(BUMP) != nullptr;
        bool lighting = (frag == bump);
        if (DebugNormal || LambertStep)
        {
            lighting = frag;
        }

        (*s)((mesh || vert) && normal > 0, "float3 worldNormal = normalize(mul(float4(normal, 0.0), world).xyz);"     );
        (*s)((mesh || vert) && normal > 1, "float3 worldTangent = normalize(mul(float4(tangent, 0.0), world).xyz);"   );
        (*s)((mesh || vert) && normal > 2, "float3 worldBinormal = normalize(mul(float4(binormal, 0.0), world).xyz);" );

        (*s)(true,        "float3 cameraPosition = uniBuffer[uniIndex++].xyz;" );
        (*s)(LambertStep, "float lambertStep = uniBuffer[uniIndex].w;"        );
        (*s)(true,        "float3 lightDirection = uniBuffer[uniIndex++].xyz;" );
        (*s)(true,        "float3 lightColor = uniBuffer[uniIndex++].xyz;"     );
        (*s)(true,        "float3 ambientColor = uniBuffer[uniIndex++].xyz;"   );
        (*s)(true,        "float3 diffuseColor = uniBuffer[uniIndex++].xyz;"   );
        (*s)(true,        "float3 emissiveColor = uniBuffer[uniIndex++].xyz;"  );
        (*s)(true,        "float3 specularColor = uniBuffer[uniIndex].xyz;"    );
        (*s)(true,        "float specularHighlight = uniBuffer[uniIndex++].w;" );

        (*s)(true,                         "float3 L = lightDirection;"                                    );
        (*s)(true,                         "float3 N = float3(1.0, 1.0, 1.0) / sqrt(3.0);"                 );
        (*s)((mesh || vert) && normal > 0, "N = worldNormal;"                                              );
        (*s)(frag && normal > 0,           "N = varyWorldNormal;"                                          );
        (*s)(frag && bump && normal > 0,   "N.z = dot(varyWorldNormal, bump.xyz);"                         );
        (*s)(frag && bump && normal > 1,   "N.x = dot(varyWorldTangent, bump.xyz);"                        );
        (*s)(frag && bump && normal > 2,   "N.y = dot(varyWorldBinormal, bump.xyz);"                       );

        (*s)(lighting,                     "float lambert = max(dot(N, L), 0.0);"                          );
        (*s)(lighting && LambertStep,      "lambert = step(lambertStep, lambert);"                         );

        (*s)(frag && Specular,             "float3 V = normalize(cameraPosition - varyWorldPosition);"     );
        (*s)(frag && Specular,             "float3 H = normalize(V + L);"                                  );
        (*s)(frag && Specular,             "float phong = pow(max(dot(N, H), 0.0001), specularHighlight);" );

        (*s)(lighting,                     "color.rgb *= (ambientColor + diffuseColor * lambert);"         );
        (*s)(frag && Specular,             "color.rgb += specularColor * phong;"                           );
        (*s)(lighting,                     "color.rgb *= lightColor;"                                      );
        (*s)(lighting,                     "color.rgb += emissiveColor;"                                   );

        (*s)(frag && DebugNormal,          "color.rgb = N;"                                                );
    }
}
//------------------------------------------------------------------------------
void Material::UpdateSkinningConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (data.mesh->Skinning == false)
        return;
    if (pointer == nullptr)
    {
        size += 75 * sizeof(xxMatrix4x3);
    }
    if (size >= 75 * sizeof(xxMatrix4x3) && pointer)
    {
        xxMatrix4x3* boneMatrix = reinterpret_cast<xxMatrix4x3*>(*pointer);
        size -= 75 * sizeof(xxMatrix4x3);
        (*pointer) += 75 * 3;

        for (auto const& data : data.node->Bones)
        {
            (*boneMatrix++) = xxMatrix4x3::FromMatrix4(data.boneMatrix);
        }
    }
    if (s)
    {
        (*s)(true, "float4 zero4 = float4(0.0, 0.0, 0.0, 0.0);"                                                                                     );
        (*s)(true, "float4 boneWeight = float4(attrBoneWeight, 1.0 - attrBoneWeight.x - attrBoneWeight.y - attrBoneWeight.z);"                      );
        (*s)(true, "int4 boneIndices = int4(attrBoneIndices) * int4(3, 3, 3, 3) + int4(uniIndex, uniIndex, uniIndex, uniIndex);"                    );
        (*s)(true, "world  = float4x4(uniBuffer[boneIndices.x], uniBuffer[boneIndices.x + 1], uniBuffer[boneIndices.x + 2], zero4) * boneWeight.x;" );
        (*s)(true, "world += float4x4(uniBuffer[boneIndices.y], uniBuffer[boneIndices.y + 1], uniBuffer[boneIndices.y + 2], zero4) * boneWeight.y;" );
        (*s)(true, "world += float4x4(uniBuffer[boneIndices.z], uniBuffer[boneIndices.z + 1], uniBuffer[boneIndices.z + 2], zero4) * boneWeight.z;" );
        (*s)(true, "world += float4x4(uniBuffer[boneIndices.w], uniBuffer[boneIndices.w + 1], uniBuffer[boneIndices.w + 2], zero4) * boneWeight.w;" );
        (*s)(true, "world[3][3] = 1.0;"                                                                                                             );
        (*s)(true, "uniIndex += 75 * 3;"                                                                                                            );

        (*s).GHM(true, "", "world = transpose(world);", "");
    }
}
//------------------------------------------------------------------------------
void Material::UpdateTransformConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (s)
    {
        (*s)(true, "float4 worldPosition = mul(float4(attrPosition, 1.0), world);"      );
        (*s)(true, "float4 screenPosition = mul(mul(worldPosition, view), projection);" );
    }
}
//------------------------------------------------------------------------------
void Material::UpdateWorldViewProjectionConstant(xxDrawData const& data, int& size, xxVector4** pointer, struct MaterialSelector* s) const
{
    if (pointer == nullptr)
    {
        size += 3 * sizeof(xxMatrix4x4);
    }
    if (size >= 3 * sizeof(xxMatrix4x4) && pointer)
    {
        xxMatrix4x4* wvp = reinterpret_cast<xxMatrix4x4*>(*pointer);
        size -= 3 * sizeof(xxMatrix4x4);
        (*pointer) += 3 * 4;

        xxCamera* camera = data.camera;
        wvp[0] = data.node->WorldMatrix;
        if (camera)
        {
            wvp[1] = camera->ViewMatrix;
            wvp[2] = camera->ProjectionMatrix;
        }
        else
        {
            wvp[1] = xxMatrix4::IDENTITY;
            wvp[2] = xxMatrix4::IDENTITY;
        }
    }
    if (s)
    {
        (*s)(true, "float4x4 world = float4x4(uniBuffer[uniIndex + 0], uniBuffer[uniIndex + 1], uniBuffer[uniIndex + 2], uniBuffer[uniIndex + 3]);"        );
        (*s)(true, "float4x4 view = float4x4(uniBuffer[uniIndex + 4], uniBuffer[uniIndex + 5], uniBuffer[uniIndex + 6], uniBuffer[uniIndex + 7]);"         );
        (*s)(true, "float4x4 projection = float4x4(uniBuffer[uniIndex + 8], uniBuffer[uniIndex + 9], uniBuffer[uniIndex + 10], uniBuffer[uniIndex + 11]);" );
        (*s)(true, "uniIndex += 12;"                                                                                                                       );

        (*s).GHM(true, "world = transpose(world);",           "", "world = transpose(world);"           );
        (*s).GHM(true, "view = transpose(view);",             "", "view = transpose(view);"             );
        (*s).GHM(true, "projection = transpose(projection);", "", "projection = transpose(projection);" );
    }
}
//------------------------------------------------------------------------------
static xxMaterialPtr (*backupBinaryCreate)();
//------------------------------------------------------------------------------
void Material::Initialize()
{
    if (backupBinaryCreate)
        return;
    backupBinaryCreate = xxMaterial::BinaryCreate;

    xxMaterial::BinaryCreate = []() -> xxMaterialPtr
    {
        xxMaterialPtr material = xxMaterialPtr(new Material(), [](Material* material) { delete material; });
        if (material == nullptr)
            return nullptr;

        return material;
    };

    DefaultMaterial = xxMaterial::Create();
}
//------------------------------------------------------------------------------
void Material::Shutdown()
{
    if (backupBinaryCreate == nullptr)
        return;
    xxMaterial::BinaryCreate = backupBinaryCreate;
    backupBinaryCreate = nullptr;

    DefaultMaterial = nullptr;
}
//------------------------------------------------------------------------------
#if defined(__clang__)
char const xxMaterial::DefaultShader[] = "";
#else
extern char const xxMaterialDefaultShader[] = "";
#pragma comment(linker, "/alternatename:?DefaultShader@xxMaterial@@2QBDB=?xxMaterialDefaultShader@@3QBDB")
#endif
//==============================================================================
