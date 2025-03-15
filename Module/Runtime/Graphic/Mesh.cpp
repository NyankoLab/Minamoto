//==============================================================================
// Minamoto : Mesh Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include "Mesh.h"

//==============================================================================
Mesh::Mesh(bool skinning, char normal, char color, char texture)
    :xxMesh(skinning, normal, color, texture)
{
}
//------------------------------------------------------------------------------
Mesh::~Mesh()
{
}
//------------------------------------------------------------------------------
void Mesh::Invalidate()
{
    xxMesh::Invalidate();
}
//------------------------------------------------------------------------------
void Mesh::Setup(uint64_t device)
{
    m_device = device;

    if (m_vertexAttribute == 0)
    {
        std::vector<std::array<int, 4>> attributes;
        int offset = 0;

        // position
        attributes.push_back({0, offset, 'POS3', xxSizeOf(xxVector3)});
        offset += xxSizeOf(xxVector3);

        // skinning
        if (Skinning)
        {
            // bone weight
            attributes.push_back({0, offset, 'BON3', xxSizeOf(xxVector3)});
            offset += xxSizeOf(xxVector3);

            // bone index
            attributes.push_back({0, offset, 'BON4', xxSizeOf(uint32_t)});
            offset += xxSizeOf(uint32_t);
        }

        // normal
        for (int i = 0; i < NormalCount; ++i)
        {
            attributes.push_back({0, offset, 'NOR3', xxSizeOf(uint32_t)});
            offset += xxSizeOf(uint32_t);
        }

        // color
        for (int i = 0; i < ColorCount; ++i)
        {
            attributes.push_back({0, offset, 'COL4', xxSizeOf(uint32_t)});
            offset += xxSizeOf(uint32_t);
        }

        // texture
        for (int i = 0; i < TextureCount; ++i)
        {
            attributes.push_back({0, offset, 'TEX2', xxSizeOf(xxVector2)});
            offset += xxSizeOf(xxVector2);
        }

        int count = (int)attributes.size();
        int* attribute = attributes.front().data();
        m_vertexAttribute = xxCreateVertexAttribute(m_device, count, attribute);
    }

    xxMesh::Setup(device);
}
//------------------------------------------------------------------------------
void Mesh::Draw(uint64_t commandEncoder, int instanceCount, int firstIndex, int vertexOffset, int firstInstance)
{
    if (Count[STORAGE0] != 0)
    {
        uint64_t buffers[4];
        buffers[0] = m_buffers[VERTEX][m_bufferIndex[VERTEX]];
        buffers[1] = m_buffers[STORAGE0][m_bufferIndex[STORAGE0]];
        buffers[2] = m_buffers[STORAGE1][m_bufferIndex[STORAGE1]];
        buffers[3] = m_buffers[STORAGE2][m_bufferIndex[STORAGE2]];
        xxSetMeshBuffers(commandEncoder, 4, buffers);
        xxDrawMeshed(commandEncoder, Count[STORAGE0], 1, 1);
        return;
    }
    xxSetVertexBuffers(commandEncoder, 1, &m_buffers[VERTEX][m_bufferIndex[VERTEX]], m_vertexAttribute);
    if (Count[INDEX] == 0)
    {
        xxDraw(commandEncoder, Count[VERTEX] - firstIndex * 3, instanceCount, vertexOffset + firstIndex * 3, firstInstance);
    }
    else
    {
        xxDrawIndexed(commandEncoder, m_buffers[INDEX][m_bufferIndex[INDEX]], Count[INDEX], Count[VERTEX], instanceCount, firstIndex, vertexOffset, firstInstance);
    }
}
//------------------------------------------------------------------------------
void Mesh::SetVertexCount(int count)
{
    if (Stride[VERTEX] == 0)
    {
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector3) * 1;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(uint32_t) * NormalCount;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(uint32_t) * ColorCount;
        const_cast<int&>(Stride[VERTEX]) += xxSizeOf(xxVector2) * TextureCount;
    }
    xxMesh::SetVertexCount(count);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> Mesh::GetNormal(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride[VERTEX], NormalCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<uint32_t> Mesh::GetColor(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * NormalCount;
    vertex += xxSizeOf(uint32_t) * index;
    return xxStrideIterator<uint32_t>(vertex, Stride[VERTEX], ColorCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
xxStrideIterator<xxVector2> Mesh::GetTexture(int index) const
{
    char* vertex = Storage[VERTEX];
    vertex += xxSizeOf(xxVector3);
    vertex += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
    vertex += xxSizeOf(uint32_t) * NormalCount;
    vertex += xxSizeOf(uint32_t) * ColorCount;
    vertex += xxSizeOf(xxVector2) * index;
    return xxStrideIterator<xxVector2>(vertex, Stride[VERTEX], TextureCount ? Count[VERTEX] : 0);
}
//------------------------------------------------------------------------------
unsigned int Mesh::GetIndex(int index) const
{
    if (Count[INDEX])
    {
        if (Count[VERTEX] < 65536)
        {
            return ((unsigned short*)Index)[index];
        }
        else
        {
            return ((unsigned int*)Index)[index];
        }
    }
    return index;
}
//------------------------------------------------------------------------------
void Mesh::BinaryRead(xxBinary& binary)
{
    xxMesh::BinaryRead(binary);

    // legacy
    if (NormalCount)
    {
        int stride = 0;
        stride += xxSizeOf(xxVector3) * 1;
        stride += xxSizeOf(xxVector3) * (Skinning ? 1 : 0);
        stride += xxSizeOf(uint32_t) * (Skinning ? 1 : 0);
        stride += xxSizeOf(uint32_t) * NormalCount;
        stride += xxSizeOf(uint32_t) * ColorCount;
        stride += xxSizeOf(xxVector2) * TextureCount;
        if (Stride[VERTEX] != stride)
        {
            auto rightPosition = xxMesh::GetPosition();
            auto rightBoneWeight = xxMesh::GetBoneWeight();
            auto rightBoneIndices = xxMesh::GetBoneIndices();
            auto rightNormal = xxMesh::GetNormal();
            auto rightColor = xxMesh::GetColor();
            auto rightTexture = xxMesh::GetTexture();

            const_cast<int&>(Stride[VERTEX]) = stride;

            auto leftPosition = GetPosition();
            auto leftBoneWeight = GetBoneWeight();
            auto leftBoneIndices = GetBoneIndices();
            auto leftNormal = GetNormal();
            auto leftColor = GetColor();
            auto leftTexture = GetTexture();

            for (int i = 0; i < VertexCount; ++i)
            {
                *leftPosition++ = *rightPosition++;
                if (Skinning)
                {
                    *leftBoneWeight++ = *rightBoneWeight++;
                    *leftBoneIndices++ = *rightBoneIndices++;
                }
                for (int i = 0; i < NormalCount; ++i)
                {
                    xxVector3* right = &(*rightNormal);
                    uint32_t* left = &(*leftNormal);
                    *(left + i) = NormalEncode(*(right + i));
                }
                rightNormal++;
                leftNormal++;
                for (int i = 0; i < ColorCount; ++i)
                {
                    uint32_t* right = &(*rightColor);
                    uint32_t* left = &(*leftColor);
                    *(left + i) = *(right + i);
                }
                rightColor++;
                leftColor++;
                for (int i = 0; i < TextureCount; ++i)
                {
                    xxVector2* right = &(*rightTexture);
                    xxVector2* left = &(*leftTexture);
                    *(left + i) = *(right + i);
                }
                rightTexture++;
                leftTexture++;
            }
        }
    }
}
//------------------------------------------------------------------------------
static xxMeshPtr (*backupBinaryCreate)();
//------------------------------------------------------------------------------
void Mesh::Initialize()
{
    if (backupBinaryCreate)
        return;
    backupBinaryCreate = xxMesh::BinaryCreate;

    xxMesh::BinaryCreate = []() -> xxMeshPtr
    {
        xxMeshPtr mesh = xxMeshPtr(new Mesh(false, 0, 0, 0), [](Mesh* mesh) { delete mesh; });
        if (mesh == nullptr)
            return nullptr;

        return mesh;
    };
}
//------------------------------------------------------------------------------
void Mesh::Shutdown()
{
    if (backupBinaryCreate == nullptr)
        return;
    xxMesh::BinaryCreate = backupBinaryCreate;
    backupBinaryCreate = nullptr;
}
//------------------------------------------------------------------------------
xxVector3 Mesh::NormalDecode(uint32_t value)
{
    xxVector3 output;
#if USE_RGB10A2
    output.x = ((value >>  0) & 0x3FF) / 511.5f - 1.0f;
    output.y = ((value >> 10) & 0x3FF) / 511.5f - 1.0f;
    output.z = ((value >> 20) & 0x3FF) / 511.5f - 1.0f;
#else
    output.x = ((value >>  0) & 0xFF) / 127.5f - 1.0f;
    output.y = ((value >>  8) & 0xFF) / 127.5f - 1.0f;
    output.z = ((value >> 16) & 0xFF) / 127.5f - 1.0f;
#endif
    return output;
}
//------------------------------------------------------------------------------
uint32_t Mesh::NormalEncode(xxVector3 const& value)
{
    uint32_t output = 0;
#if USE_RGB10A2
    output |= (uint32_t(value.x * 511.5f + 511.5f) & 0x3FF) <<  0;
    output |= (uint32_t(value.y * 511.5f + 511.5f) & 0x3FF) << 10;
    output |= (uint32_t(value.z * 511.5f + 511.5f) & 0x3FF) << 20;
#else
    output |= (uint32_t(value.x * 127.5f + 127.5f) & 0xFF) <<  0;
    output |= (uint32_t(value.y * 127.5f + 127.5f) & 0xFF) <<  8;
    output |= (uint32_t(value.z * 127.5f + 127.5f) & 0xFF) << 16;
#endif
    return output;
}
//==============================================================================
