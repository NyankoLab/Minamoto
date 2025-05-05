//==============================================================================
// Minamoto : Mesh Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"
#include <xxGraphicPlus/xxMesh.h>

struct RuntimeAPI Mesh : public xxMesh
{
public:
    void                        Invalidate();
    void                        Setup(uint64_t device);
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0);

    void                        SetVertexCount(int count);

    xxStrideIterator<uint32_t>  GetNormal(int index = 0) const;
    xxStrideIterator<uint32_t>  GetColor(int index = 0) const;
    xxStrideIterator<xxVector2> GetTexture(int index = 0) const;

    unsigned int                GetIndex(int index) const;

    void                        BinaryRead(xxBinary& binary) override;

protected:
    Mesh(bool skinning, char normal, char color, char texture);
    virtual ~Mesh();

public:
    static void                 Initialize();
    static void                 Shutdown();

    static xxVector3            NormalDecode(uint32_t value);
    static uint32_t             NormalEncode(xxVector3 const& value);
};

#if defined(xxWINDOWS)
#if defined(_LIBCPP_VERSION)
#pragma comment(linker, "/alternatename:__imp_?Create@xxMesh@@SA?AV?$shared_ptr@UMesh@@@__1@std@@_NDDD@Z=__imp_?Create@xxMesh@@SA?AV?$shared_ptr@VxxMesh@@@__1@std@@_NDDD@Z")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxMesh@@2P6A?AV?$shared_ptr@UMesh@@@__1@std@@XZEA=?BinaryCreate@xxMesh@@2P6A?AV?$shared_ptr@VxxMesh@@@__1@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxMesh@@SA?AV?$shared_ptr@UMesh@@@__1@std@@_NDDD@Z=?Create@xxMesh@@SA?AV?$shared_ptr@VxxMesh@@@__1@std@@_NDDD@Z")
#else
#pragma comment(linker, "/alternatename:__imp_?Create@xxMesh@@SA?AV?$shared_ptr@UMesh@@@std@@_NDDD@Z=__imp_?Create@xxMesh@@SA?AV?$shared_ptr@VxxMesh@@@std@@_NDDD@Z")
#pragma comment(linker, "/alternatename:?BinaryCreate@xxMesh@@2P6A?AV?$shared_ptr@UMesh@@@std@@XZEA=?BinaryCreate@xxMesh@@2P6A?AV?$shared_ptr@VxxMesh@@@std@@XZEA")
#pragma comment(linker, "/alternatename:?Create@xxMesh@@SA?AV?$shared_ptr@UMesh@@@std@@_NDDD@Z=?Create@xxMesh@@SA?AV?$shared_ptr@VxxMesh@@@std@@_NDDD@Z")
#endif
#endif
