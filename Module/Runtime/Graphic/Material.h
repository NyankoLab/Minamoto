//==============================================================================
// Minamoto : Material Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxMaterial.h>

struct RuntimeAPI Material : public xxMaterial
{
public:
    enum TextureType
    {
        BASE                = 0,
        BUMP                = 1,
    };

public:
    void                    Invalidate() override;
    void                    Draw(xxDrawData const& data) const override;

    void                    CreatePipeline(xxDrawData const& data) override;
    void                    CreateConstant(xxDrawData const& data) const override;
    void                    UpdateConstant(xxDrawData const& data) const override;

protected:
    std::string             GetShader(xxDrawData const& data, int type) const override;
    int                     GetMeshConstantSize(xxDrawData const& data) const;
    int                     GetVertexConstantSize(xxDrawData const& data) const override;
    int                     GetFragmentConstantSize(xxDrawData const& data) const override;

    void                    ShaderDefault(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderAttribute(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderConstant(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderVarying(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderMesh(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderVertex(xxDrawData const& data, struct MaterialSelector& s) const;
    void                    ShaderFragment(xxDrawData const& data, struct MaterialSelector& s) const;

    void                    UpdateAlphaTestingConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateBlendingConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateCullingConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateLightingConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateSkinningConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateTransformConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;
    void                    UpdateWorldViewProjectionConstant(xxDrawData const& data, int& size, xxVector4** pointer = nullptr, struct MaterialSelector* s = nullptr) const;

    uint64_t                m_meshShader = 0;
    uint16_t                m_meshTextureSlot = 0;
    uint16_t                m_vertexTextureSlot = 0;
    uint16_t                m_fragmentTextureSlot = 0;

public:
    bool                    LambertRound = false;

    bool                    BackfaceCulling = false;
    bool                    FrustumCulling = false;

    bool                    DebugNormal = false;

    static void             Initialize();
    static void             Shutdown();
};

#if defined(xxWINDOWS)
#pragma comment(linker, "/alternatename:__imp_?Create@xxMaterial@@SA?AV?$shared_ptr@UMaterial@@@__1@std@@XZ=__imp_?Create@xxMaterial@@SA?AV?$shared_ptr@VxxMaterial@@@__1@std@@XZ")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxMaterial@@2P6A?AV?$shared_ptr@UMaterial@@@__1@std@@XZEA=?BinaryCreate@xxMaterial@@2P6A?AV?$shared_ptr@VxxMaterial@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxMaterial@@SA?AV?$shared_ptr@UMaterial@@@__1@std@@XZ=?Create@xxMaterial@@SA?AV?$shared_ptr@VxxMaterial@@@__1@std@@XZ")
#endif
