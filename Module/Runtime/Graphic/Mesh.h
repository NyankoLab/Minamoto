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
    void                        Invalidate() override;
    void                        Setup(uint64_t device) override;
    void                        Draw(uint64_t commandEncoder, int instanceCount = 1, int firstIndex = 0, int vertexOffset = 0, int firstInstance = 0) override;

    void                        SetVertexCount(int count);

    xxStrideIterator<uint32_t>  GetNormal(int index = 0) const;
    xxStrideIterator<uint32_t>  GetColor(int index = 0) const;
    xxStrideIterator<xxVector2> GetTexture(int index = 0) const;

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
