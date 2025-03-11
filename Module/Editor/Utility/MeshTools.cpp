//==============================================================================
// Minamoto : MeshTools Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include "MeshTools.h"

#define TAG "MeshTools"

//==============================================================================
std::vector<uint32_t> MeshTools::GetIndexFromMesh(xxMeshPtr const& mesh)
{
    std::vector<uint32_t> indices;
    if (mesh == nullptr)
        return indices;

    if (mesh->Count[xxMesh::VERTEX] < 65536)
    {
        auto inputIndices = (uint16_t*)mesh->Index;
        for (int i = 0; i < mesh->Count[xxMesh::INDEX]; ++i)
            indices.push_back(*inputIndices++);
    }
    else
    {
        auto inputIndices = (uint32_t*)mesh->Index;
        for (int i = 0; i < mesh->Count[xxMesh::INDEX]; ++i)
            indices.push_back(*inputIndices++);
    }

    return indices;
}
//------------------------------------------------------------------------------
void MeshTools::SetIndexToMesh(xxMeshPtr const& mesh, std::vector<uint32_t> const& indices)
{
    if (mesh == nullptr)
        return;

    mesh->SetIndexCount(static_cast<int>(indices.size()));
    if (mesh->Count[xxMesh::VERTEX] < 65536)
    {
        auto outputIndices = (uint16_t*)mesh->Index;
        for (size_t i = 0; i < indices.size(); ++i)
            (*outputIndices++) = indices[i];
    }
    else
    {
        auto outputIndices = (uint32_t*)mesh->Index;
        for (size_t i = 0; i < indices.size(); ++i)
            (*outputIndices++) = indices[i];
    }
}
//------------------------------------------------------------------------------
MeshTools::MeshData MeshTools::CreateMeshDataFromMesh(xxMeshPtr const& mesh)
{
    MeshTools::MeshData data;
    if (mesh == nullptr)
        return data;

    data.skinning = mesh->Skinning;
    data.normalCount = mesh->NormalCount;
    data.colorCount = mesh->ColorCount;
    data.textureCount = mesh->TextureCount;
    if (data.normalCount > 8 || data.colorCount > 8 || data.textureCount > 8)
    {
        xxLog(TAG, "ExtractFromMesh failed (%d,%d,%d)", data.normalCount, data.colorCount, data.textureCount);
        return data;
    }

    xxStrideIterator<xxVector3> inputPositions = mesh->GetPosition();
    xxStrideIterator<xxVector3> inputBoneWeight = mesh->GetBoneWeight();
    xxStrideIterator<uint32_t> inputBoneIndices = mesh->GetBoneIndices();
    xxStrideIterator<uint32_t> inputNormals[8] =
    {
        mesh->GetNormal(0), mesh->GetNormal(1), mesh->GetNormal(2), mesh->GetNormal(3),
        mesh->GetNormal(4), mesh->GetNormal(5), mesh->GetNormal(6), mesh->GetNormal(7),
    };
    xxStrideIterator<uint32_t> inputColors[8] =
    {
        mesh->GetColor(0), mesh->GetColor(1), mesh->GetColor(2), mesh->GetColor(3),
        mesh->GetColor(4), mesh->GetColor(5), mesh->GetColor(6), mesh->GetColor(7),
    };
    xxStrideIterator<xxVector2> inputTextures[8] =
    {
        mesh->GetTexture(0), mesh->GetTexture(1), mesh->GetTexture(2), mesh->GetTexture(3),
        mesh->GetTexture(4), mesh->GetTexture(5), mesh->GetTexture(6), mesh->GetTexture(7),
    };

    data.indices = GetIndexFromMesh(mesh);

    for (int i = 0; i < mesh->Count[xxMesh::VERTEX]; ++i)
    {
        data.positions.push_back(*inputPositions++);
        if (data.skinning)
        {
            data.boneWeights.push_back(*inputBoneWeight++);
            data.boneIndices.push_back(*inputBoneIndices++);
        }
        for (int j = 0; j < data.normalCount; ++j)
            data.normals.push_back(*inputNormals[j]++);
        for (int j = 0; j < data.colorCount; ++j)
            data.colors.push_back(*inputColors[j]++);
        for (int j = 0; j < data.textureCount; ++j)
            data.textures.push_back(*inputTextures[j]++);
    }

    for (int i = 0; i < 6; ++i)
    {
        data.storageCount[i] = mesh->Count[i];
        data.storageStride[i] = mesh->Stride[i];
        data.storages[i].assign(mesh->Storage[i], mesh->Storage[i] + mesh->Count[i] * mesh->Stride[i]);
    }

    return data;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::CreateMeshFromMeshData(MeshData const& data)
{
    xxMeshPtr output = xxMesh::Create(data.skinning, data.normalCount, data.colorCount, data.textureCount);
    output->SetIndexCount(static_cast<int>(data.indices.size()));
    output->SetVertexCount(static_cast<int>(data.positions.size()));
    for (int i = 0; i < 6; ++i)
    {
        output->SetStorageCount(i, data.storageCount[i], data.storageStride[i]);
    }

    xxStrideIterator<xxVector3> outputPositions = output->GetPosition();
    xxStrideIterator<xxVector3> outputBoneWeight = output->GetBoneWeight();
    xxStrideIterator<uint32_t> outputBoneIndices = output->GetBoneIndices();
    xxStrideIterator<uint32_t> outputNormals[8] =
    {
        output->GetNormal(0), output->GetNormal(1), output->GetNormal(2), output->GetNormal(3),
        output->GetNormal(4), output->GetNormal(5), output->GetNormal(6), output->GetNormal(7),
    };
    xxStrideIterator<uint32_t> outputColors[8] =
    {
        output->GetColor(0), output->GetColor(1), output->GetColor(2), output->GetColor(3),
        output->GetColor(4), output->GetColor(5), output->GetColor(6), output->GetColor(7),
    };
    xxStrideIterator<xxVector2> outputTextures[8] =
    {
        output->GetTexture(0), output->GetTexture(1), output->GetTexture(2), output->GetTexture(3),
        output->GetTexture(4), output->GetTexture(5), output->GetTexture(6), output->GetTexture(7),
    };

    SetIndexToMesh(output, data.indices);

    for (size_t i = 0; i < data.positions.size(); ++i)
    {
        (*outputPositions++) = data.positions[i];
        if (data.skinning)
        {
            (*outputBoneWeight++) = data.boneWeights[i];
            (*outputBoneIndices++) = data.boneIndices[i];
        }
        for (int j = 0; j < data.normalCount; ++j)
            (*outputNormals[j]++) = data.normals[i * data.normalCount + j];
        for (int j = 0; j < data.colorCount; ++j)
            (*outputColors[j]++) = data.colors[i * data.colorCount + j];
        for (int j = 0; j < data.textureCount; ++j)
            (*outputTextures[j]++) = data.textures[i * data.textureCount + j];
    }

    for (int i = 0; i < 6; ++i)
    {
        memcpy(output->Storage[i], data.storages[i].data(), data.storages[i].size());
    }

    output->CalculateBound();

    return output;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::CreateMesh(std::vector<xxVector3> const& vertices,
                                std::vector<xxVector3> const& boneWeights,
                                std::vector<uint32_t> const& boneIndices,
                                std::vector<xxVector3> const& normals,
                                std::vector<xxVector4> const& colors,
                                std::vector<xxVector2> const& textures,
                                std::vector<uint32_t> const& indices)
{
    if (vertices.empty())
        return nullptr;

    MeshTools::MeshData data;
    data.skinning = boneWeights.size() && boneIndices.size();
    data.normalCount = int(normals.size() / vertices.size());
    data.colorCount = int(colors.size() / vertices.size());
    data.textureCount = int(textures.size() / vertices.size());

    data.positions = vertices;
    data.boneWeights = boneWeights;
    data.boneIndices = boneIndices;
//  data.normals = normals;
    data.normals.reserve(normals.size());
    for (xxVector3 const& normal : normals)
    {
        data.normals.push_back(Mesh::NormalEncode(normal));
    }
//  data.colors = colors;
    data.colors.reserve(colors.size());
    for (xxVector4 const& color : colors)
    {
        data.colors.push_back(color.ToInteger());
    }
    data.textures = textures;
    data.indices = indices;

    data.boneWeights.resize(vertices.size());
    data.boneIndices.resize(vertices.size());
    data.normals.resize(vertices.size());
    data.colors.resize(vertices.size());
    data.textures.resize(vertices.size());

    return CreateMeshFromMeshData(data);
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::CreateMeshlet(xxMeshPtr const& mesh)
{
    if (mesh == nullptr)
        return nullptr;
    if (mesh->Storage[xxMesh::STORAGE0])
        return mesh;

    float begin = xxGetCurrentTime();

    const size_t max_vertices = 64;
    const size_t max_triangles = 124;
    const float cone_weight = 0.0f;

    std::vector<uint32_t> indices = GetIndexFromMesh(mesh);
    size_t max_meshlets = meshopt_buildMeshletsBound(indices.size(), max_vertices, max_triangles);
    std::vector<meshopt_Meshlet> meshlets(max_meshlets);
    std::vector<uint32_t> meshlet_vertices(max_meshlets * max_vertices);
    std::vector<uint8_t> meshlet_triangles(max_meshlets * max_triangles * 3);

    size_t meshlet_count = meshopt_buildMeshlets(meshlets.data(), meshlet_vertices.data(), meshlet_triangles.data(),
                                                 indices.data(), indices.size(),
                                                 (float*)mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride,
                                                 max_vertices, max_triangles, cone_weight);

    struct MeshletStorage
    {
        uint32_t vertex_offset;
        uint32_t triangle_offset;
        uint32_t vertex_count;
        uint32_t triangle_count;
        xxVector4 center_radius;
        xxVector4 cone_apex;
        xxVector4 cone_axis_cutoff;
    };
    static_assert(sizeof(MeshletStorage) == 64);

    uint32_t vertex_count = 0;
    uint32_t triangle_count = 0;
    std::vector<MeshletStorage> storages;
    std::vector<uint32_t> meshlet_triangles32(max_meshlets * max_triangles);
    for (size_t i = 0; i < meshlet_count; ++i)
    {
        meshopt_Meshlet const& meshlet = meshlets[i];

        vertex_count = std::max(vertex_count, meshlet.vertex_offset + meshlet.vertex_count);
        triangle_count = std::max(triangle_count, meshlet.triangle_offset / 3 + meshlet.triangle_count);

        meshopt_Bounds bounds = meshopt_computeMeshletBounds(&meshlet_vertices[meshlet.vertex_offset],
                                                             &meshlet_triangles[meshlet.triangle_offset],
                                                             meshlet.triangle_count,
                                                             (float*)mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride);

        // Meshlet Storage
        MeshletStorage storage;
        storage.vertex_offset = meshlet.vertex_offset;
        storage.triangle_offset = meshlet.triangle_offset / 3;
        storage.vertex_count = meshlet.vertex_count;
        storage.triangle_count = meshlet.triangle_count;
        storage.center_radius = { bounds.center[0], bounds.center[1], bounds.center[2], bounds.radius };
        storage.cone_apex = { bounds.cone_apex[0], bounds.cone_apex[1], bounds.cone_apex[2] };
        storage.cone_axis_cutoff = { bounds.cone_axis[0], bounds.cone_axis[1], bounds.cone_axis[2], bounds.cone_cutoff };
        storages.push_back(storage);

        for (uint32_t i = 0; i < meshlet.triangle_count; ++i)
        {
            uint32_t indices = 0;
            indices |= meshlet_triangles[meshlet.triangle_offset + i * 3 + 0] <<  0;
            indices |= meshlet_triangles[meshlet.triangle_offset + i * 3 + 1] <<  8;
            indices |= meshlet_triangles[meshlet.triangle_offset + i * 3 + 2] << 16;
            meshlet_triangles32[storage.triangle_offset + i] = indices;
        }
    }

    meshlets.resize(meshlet_count);
    meshlet_vertices.resize(vertex_count);
    meshlet_triangles.resize(triangle_count * 3);
    meshlet_triangles32.resize(triangle_count);

    mesh->SetStorageCount(xxMesh::STORAGE0, static_cast<int>(storages.size()), xxSizeOf(MeshletStorage));
    mesh->SetStorageCount(xxMesh::STORAGE1, static_cast<int>(meshlet_vertices.size()), xxSizeOf(uint32_t));
    mesh->SetStorageCount(xxMesh::STORAGE2, static_cast<int>(meshlet_triangles32.size()), xxSizeOf(uint32_t));
    memcpy(mesh->Storage[xxMesh::STORAGE0], storages.data(), storages.size() * xxSizeOf(MeshletStorage));
    memcpy(mesh->Storage[xxMesh::STORAGE1], meshlet_vertices.data(), meshlet_vertices.size() * xxSizeOf(uint32_t));
    memcpy(mesh->Storage[xxMesh::STORAGE2], meshlet_triangles32.data(), meshlet_triangles32.size() * xxSizeOf(uint32_t));

    xxLog(TAG, "CreateMeshlet : %s (%.0fus)", mesh->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);

    return mesh;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::IndexingMesh(xxMeshPtr const& mesh)
{
    if (mesh == nullptr)
        return nullptr;
    if (mesh->Index)
        return mesh;

    float begin = xxGetCurrentTime();

    MeshData data = CreateMeshDataFromMesh(mesh);
    if (data.positions.empty())
        return mesh;

    data.indices.clear();
    for (uint32_t i = 0; i < data.positions.size(); ++i)
    {
        data.indices.push_back(i);
    }

    for (size_t i = 0; i < data.positions.size(); ++i)
    {
        auto compare = [](auto& container, size_t a, size_t b, int c)
        {
            for (int i = 0; i < c; ++i)
                if (container[a * c + i] != container[b * c + i])
                    return false;
            return true;
        };

        auto remove = [](auto& container, size_t a, int c)
        {
            for (int i = 0; i < c; ++i)
                container[a * c + i] = container[container.size() - c + i];
            container.pop_back();
        };

        auto replace = [](auto& container, size_t from, size_t to)
        {
            for (size_t i = std::min(from, to); i < container.size(); ++i)
                if (container[i] == from)
                    container[i] = decltype(container[i])(to);
        };

        for (size_t j = i + 1; j < data.positions.size(); ++j)
        {
            if (compare(data.positions, i, j, 1) == false)
                continue;
            if (compare(data.boneWeights, i, j, data.skinning ? 1 : 0) == false)
                continue;
            if (compare(data.boneIndices, i, j, data.skinning ? 1 : 0) == false)
                continue;
            if (compare(data.normals, i, j, data.normalCount) == false)
                continue;
            if (compare(data.colors, i, j, data.colorCount) == false)
                continue;
            if (compare(data.textures, i, j, data.textureCount) == false)
                continue;
            remove(data.positions, j, 1);
            remove(data.boneWeights, j, data.skinning ? 1 : 0);
            remove(data.boneIndices, j, data.skinning ? 1 : 0);
            remove(data.normals, j, data.normalCount);
            remove(data.colors, j, data.colorCount);
            remove(data.textures, j, data.textureCount);
            replace(data.indices, j, i);
            replace(data.indices, data.positions.size(), j);
            --j;
        }
    }

    xxMeshPtr output = CreateMeshFromMeshData(data);
    output->Name = mesh->Name;

    xxLog(TAG, "OptimizeMesh : %s Vertex count from %d to %d and index count %d (%.0fus)", mesh->Name.c_str(), mesh->Count[xxMesh::VERTEX], output->Count[xxMesh::VERTEX], output->Count[xxMesh::INDEX], (xxGetCurrentTime() - begin) * 1000000);

    return output;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::NormalizeMesh(xxMeshPtr const& mesh, bool tangent)
{
    if (mesh == nullptr)
        return nullptr;
    if (tangent == false && mesh->NormalCount == 1)
        return mesh;
    if (tangent == true && mesh->NormalCount == 3)
        return mesh;

    float begin = xxGetCurrentTime();

    MeshData data = CreateMeshDataFromMesh(mesh);
    if (data.positions.empty())
        return mesh;

    data.normalCount = tangent ? 3 : 1;
    std::vector<xxVector3> normals(data.positions.size() * data.normalCount);
    size_t count = data.indices.size();
    if (count == 0)
        count = data.positions.size();
    for (uint32_t i = 0; i < count; i += 3)
    {
        uint32_t i0 = data.indices.empty() ? (i + 0) : data.indices[i + 0];
        uint32_t i1 = data.indices.empty() ? (i + 1) : data.indices[i + 1];
        uint32_t i2 = data.indices.empty() ? (i + 2) : data.indices[i + 2];

        xxVector3 x = data.positions[i1] - data.positions[i0];
        xxVector3 y = data.positions[i2] - data.positions[i0];
        xxVector3 n = xxVector3::Z;
        xxVector3 t = xxVector3::Y;
        xxVector3 b = xxVector3::X;

        if (mesh->NormalCount == 0)
        {
            n = x.Cross(y);
        }
        else
        {
            n = Mesh::NormalDecode(data.normals[i0]);
            n += Mesh::NormalDecode(data.normals[i1]);
            n += Mesh::NormalDecode(data.normals[i2]);
        }

        if (data.normalCount == 3 && data.textureCount)
        {
            float u = data.textures[data.textureCount * i1].y - data.textures[data.textureCount * i0].y;
            float v = data.textures[data.textureCount * i2].y - data.textures[data.textureCount * i0].y;
            t = x * v - y * u;
            b = n.Cross(t);
        }

        if (mesh->NormalCount == 0)
        {
            normals[data.normalCount * i0 + 0] += n;
            normals[data.normalCount * i1 + 0] += n;
            normals[data.normalCount * i2 + 0] += n;
        }
        else if (mesh->NormalCount == 1)
        {
            normals[data.normalCount * i0 + 0] = Mesh::NormalDecode(data.normals[i0]);
            normals[data.normalCount * i1 + 0] = Mesh::NormalDecode(data.normals[i1]);
            normals[data.normalCount * i2 + 0] = Mesh::NormalDecode(data.normals[i2]);
        }
        if (data.normalCount == 3)
        {
            normals[data.normalCount * i0 + 1] += t;
            normals[data.normalCount * i1 + 1] += t;
            normals[data.normalCount * i2 + 1] += t;
            normals[data.normalCount * i0 + 2] += b;
            normals[data.normalCount * i1 + 2] += b;
            normals[data.normalCount * i2 + 2] += b;
        }
    }
    data.normals.clear();
    for (xxVector3& n : normals)
    {
        float l = n.Length();
        if (l != 0.0f)
            n /= l;
        data.normals.push_back(Mesh::NormalEncode(n));
    }

    xxMeshPtr output = CreateMeshFromMeshData(data);
    output->Name = mesh->Name;

    xxLog(TAG, "NormalizeMesh : %s (%.0fus)", mesh->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);

    return output;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::OptimizeMesh(xxMeshPtr const& mesh)
{
    if (mesh == nullptr)
        return nullptr;

    float begin = xxGetCurrentTime();

    if (mesh->Count[xxMesh::INDEX] == 0)
    {
        std::vector<unsigned int> remap(mesh->Count[xxMesh::VERTEX]);
        size_t vertex_count = meshopt_generateVertexRemap(remap.data(), nullptr, remap.size(),
                                                          mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride);

        std::vector<unsigned int> indices(mesh->Count[xxMesh::VERTEX]);
        std::vector<char> vertices(vertex_count * mesh->VertexStride);
        meshopt_remapIndexBuffer(indices.data(), nullptr, indices.size(), remap.data());
        meshopt_remapVertexBuffer(vertices.data(), mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride, remap.data());

        mesh->SetVertexCount(static_cast<int>(vertex_count));

        SetIndexToMesh(mesh, indices);
        memcpy(mesh->Vertex, vertices.data(), vertex_count * mesh->VertexStride);
    }

    std::vector<unsigned int> indices = GetIndexFromMesh(mesh);
    std::vector<char> vertices(mesh->Count[xxMesh::VERTEX] * mesh->VertexStride);
    meshopt_optimizeVertexCache(indices.data(), indices.data(), indices.size(), mesh->Count[xxMesh::VERTEX]);
    meshopt_optimizeOverdraw(indices.data(), indices.data(), indices.size(),
                             (float*)mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride, 1.05f);
    size_t vertex_count = meshopt_optimizeVertexFetch(vertices.data(), indices.data(), indices.size(),
                                                      mesh->Vertex, mesh->Count[xxMesh::VERTEX], mesh->VertexStride);

    mesh->SetVertexCount(static_cast<int>(vertex_count));

    SetIndexToMesh(mesh, indices);
    memcpy(mesh->Vertex, vertices.data(), vertex_count * mesh->VertexStride);

    xxLog(TAG, "OptimizeMesh : %s (%.0fus)", mesh->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);

    if (mesh->Count[xxMesh::STORAGE0])
    {
        mesh->SetStorageCount(xxMesh::STORAGE0, 0, 0);
        mesh->SetStorageCount(xxMesh::STORAGE1, 0, 0);
        mesh->SetStorageCount(xxMesh::STORAGE2, 0, 0);
        return CreateMeshlet(mesh);
    }

    return mesh;
}
//------------------------------------------------------------------------------
xxMeshPtr MeshTools::ResetMesh(xxMeshPtr const& mesh, xxVector3& origin)
{
    if (mesh == nullptr)
        return nullptr;

    float begin = xxGetCurrentTime();

    MeshData data = CreateMeshDataFromMesh(mesh);
    if (data.positions.empty())
        return mesh;

    xxVector3 min = { FLT_MAX, FLT_MAX, FLT_MAX };
    xxVector3 max = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
    for (auto& position : data.positions)
    {
        min = min.Minimum(position);
        max = max.Maximum(position);
    }

    xxVector3 from = origin;
    xxVector3 to = { (min.x + max.x) / 2.0f, (min.y + max.y) / 2.0f, min.z };
    xxVector3 diff = to - from;
    for (auto& position : data.positions)
    {
        position -= diff;
    }
    origin = to;

    xxMeshPtr output = CreateMeshFromMeshData(data);
    output->Name = mesh->Name;

    xxLog(TAG, "ResetMesh : %s (%.0fus)", mesh->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);

    if (mesh->Storage[xxMesh::STORAGE0])
    {
        output = CreateMeshlet(output);
    }

    return output;
}
//------------------------------------------------------------------------------
void MeshTools::UnifyMesh(xxNodePtr const& node, float threshold)
{
    std::vector<xxMeshPtr> meshes;
    Node::Traversal(node, [&](xxNodePtr const& node)
    {
        if (node->Mesh == nullptr)
            return true;

        float begin = xxGetCurrentTime();

        auto left = node->Mesh->GetPosition();
        int vertexCount = node->Mesh->VertexCount;
        for (xxMeshPtr const& mesh : meshes)
        {
            if (node->Mesh == mesh)
                break;
            if (vertexCount != mesh->VertexCount)
                continue;
            auto right = mesh->GetPosition();
            for (int i = 0; i < 4; ++i)
            {
                left = left.begin();
                right = right.begin();
                bool equal = true;
                for (int x = 0; x < vertexCount; ++x)
                {
                    xxVector3 a = *left++;
                    xxVector3 b = *right++;
                    switch (i)
                    {
                    case 0: a = { a.x, a.y, a.z };   break;
                    case 1: a = { -a.y, a.x, a.z };  break;
                    case 2: a = { -a.x, -a.y, a.z }; break;
                    case 3: a = { a.y, -a.x, a.z };  break;
                    }
                    if (std::fabsf(a.x - b.x) > threshold ||
                        std::fabsf(a.y - b.y) > threshold ||
                        std::fabsf(a.z - b.z) > threshold)
                    {
                        equal = false;
                        break;
                    }
                }
                if (equal == false)
                    continue;

                xxLog(TAG, "UnifyMesh : %s == %s (%.0fus)", node->Mesh->Name.c_str(), mesh->Name.c_str(), (xxGetCurrentTime() - begin) * 1000000);

                node->Mesh = mesh;

                static xxMatrix4 const rotateMatrix[4] =
                {
                    { xxVector4::X, xxVector4::Y, xxVector4::Z, xxVector4::W },
                    { -xxVector4::Y, xxVector4::X, xxVector4::Z, xxVector4::W },
                    { -xxVector4::X, -xxVector4::Y, xxVector4::Z, xxVector4::W },
                    { xxVector4::Y, -xxVector4::X, xxVector4::Z, xxVector4::W },
                };
                node->LocalMatrix = node->LocalMatrix * rotateMatrix[i];
                break;
            }
        }
        meshes.push_back(node->Mesh);
        return true;
    });
}
//==============================================================================
