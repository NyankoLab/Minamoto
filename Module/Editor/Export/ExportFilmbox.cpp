//==============================================================================
// Minamoto : ExportFilmbox Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include "ExportFilmbox.h"

#define FBXVersion 7100
#define TAG "ExportFilmbox"
#define TAB "\t"
#define SIZE 256

//==============================================================================
#define HAVE_RESET_SKINNING_MATRIX  1
#if (FBXVersion >= 7000)
#define HASH_NODE_ATTRIBUTE         1
#define HASH_SKIN                   1
typedef std::vector<std::tuple<size_t, size_t, std::string>> Connections;
#else
typedef std::vector<std::tuple<std::string, std::string, std::string>> Connections;
#endif
//------------------------------------------------------------------------------
static void fbx_geometry(xxFile* file, char* line, xxNodePtr const& node, Connections& connections)
{
    xxMeshPtr const& mesh = node->Mesh;

    // Mesh
    if (mesh == nullptr)
        return;

#if (FBXVersion >= 7000)
    size_t modelHash = size_t(node.get());
    size_t geometryHash = size_t(mesh.get());
    connections.emplace_back(geometryHash, modelHash, "");
    file->Write(line, snprintf(line, SIZE, TAB "Geometry: %zd, \"Geometry::\", \"Mesh\" {\n", geometryHash));
#endif

    // Vertices
    if (mesh->VertexCount)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "Vertices: *%d {\n", mesh->VertexCount));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
        file->Write(line, snprintf(line, SIZE, TAB TAB "Vertices: "));
#endif
        for (auto vertices = mesh->GetPosition(); vertices != vertices.end(); ++vertices)
        {
            xxVector3 vertex = (*vertices);
            file->Write(line, snprintf(line, SIZE, "%s%.9g,%.9g,%.9g",
                                       vertices == vertices.begin() ? "" : ",",
                                       vertex.x, vertex.y, vertex.z));
        }
        file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif
    }

    // PolygonVertexIndex
    if (mesh->IndexCount)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "PolygonVertexIndex: *%d {\n", mesh->VertexCount));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
        file->Write(line, snprintf(line, SIZE, TAB TAB "PolygonVertexIndex: "));
#endif
        for (int i = 0; i < mesh->IndexCount; i += 3)
        {
            int i0 = mesh->GetIndex(i + 0);
            int i1 = mesh->GetIndex(i + 1);
            int i2 = mesh->GetIndex(i + 2);
            file->Write(line, snprintf(line, SIZE, "%s%d,%d,%d",
                                       i == 0 ? "" : ",",
                                       i0, i1, ~i2));
        }
        file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif
    }

    // LayerElementNormal
    if (mesh->NormalCount)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementNormal: 0 {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Normals: *%d {\n", mesh->VertexCount));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB TAB "a: "));
#else
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementNormal: {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Normals: "));
#endif
        for (auto normals = mesh->GetNormal(); normals != normals.end(); ++normals)
        {
            xxVector3 normal = Mesh::NormalDecode(*normals);
            file->Write(line, snprintf(line, SIZE, "%s%.9g,%.9g,%.9g",
                                       normals == normals.begin() ? "" : ",",
                                       normal.x, normal.y, normal.z));
        }
        file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "}\n"));
#endif
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
    }

    // LayerElementColor
    if (mesh->ColorCount)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementColor: 0 {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Colors: *%d {\n", mesh->VertexCount));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB TAB "a: "));
#else
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementColor: {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Colors: "));
#endif
        for (auto colors = mesh->GetColor(); colors != colors.end(); ++colors)
        {
            xxVector4 color = xxVector4::FromInteger(*colors);
            file->Write(line, snprintf(line, SIZE, "%s%.9g,%.9g,%.9g,%.9g",
                                       colors == colors.begin() ? "" : ",",
                                       color.x, color.y, color.z, color.w));
        }
        file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "}\n"));
#endif
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
    }

    // LayerElementUV
    if (mesh->TextureCount)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementUV: 0 {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "UV: *%d {\n", mesh->VertexCount));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB TAB "a: "));
#else
        file->Write(line, snprintf(line, SIZE, TAB TAB "LayerElementUV: {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "UV: "));
#endif
        for (auto textures = mesh->GetTexture(); textures != textures.end(); ++textures)
        {
            xxVector2 texture = (*textures);
            file->Write(line, snprintf(line, SIZE, "%s%.9g,%.9g",
                                       textures == textures.begin() ? "" : ",",
                                       texture.x, 1.0f - texture.y));
        }
        file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "}\n"));
#endif
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
    }

#if (FBXVersion >= 7000)
    file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#endif
}
//------------------------------------------------------------------------------
static void fbx_material(xxFile* file, char* line, xxNodePtr const& node, Connections& connections)
{
    xxMaterialPtr const& material = node->Material;
    xxMeshPtr const& mesh = node->Mesh;

    // Material
    if (material == nullptr || mesh == nullptr)
        return;

    std::string modelName = "Model::" + node->Name;
    std::string materialName = "Material::" + material->Name;
#if (FBXVersion >= 7000)
    size_t modelHash = size_t(node.get());
    size_t materialHash = size_t(material.get());
    bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == materialHash; }) != connections.end();
    connections.emplace_back(materialHash, modelHash, "");
#else
    bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == materialName; }) != connections.end();
    connections.emplace_back(materialName, modelName, "");
#endif
    if (found == false)
    {
#if (FBXVersion >= 7000)
        file->Write(line, snprintf(line, SIZE, TAB "Material: %zd, \"%s\", \"\" {\n", materialHash, materialName.c_str()));
        file->Write(line, snprintf(line, SIZE, TAB TAB "ShadingModel: \"%s\"\n", material->Specular ? "phong" : "lambert"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "Properties70: {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"AmbientColor\", \"Color\", \"\", \"A\", %.9g, %.9g, %.9g\n", material->AmbientColor.x, material->AmbientColor.y, material->AmbientColor.z));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"DiffuseColor\", \"Color\", \"\", \"A\", %.9g, %.9g, %.9g\n", material->DiffuseColor.x, material->DiffuseColor.y, material->DiffuseColor.z));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"EmissiveColor\", \"Color\", \"\", \"A\", %.9g, %.9g, %.9g\n", material->EmissiveColor.x, material->EmissiveColor.y, material->EmissiveColor.z));
        if (material->Specular)
        {
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"SpecularColor\", \"Color\", \"\", \"A\", %.9g, %.9g, %.9g\n", material->SpecularColor.x, material->SpecularColor.y, material->SpecularColor.z));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"ShininessExponent\", \"Number\", \"\", \"A\", %.9g\n", material->SpecularHighlight));
        }
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"TransparencyFactor\", \"Number\", \"\", \"A\", %.9g\n", material->Opacity));
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
        file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#else
        file->Write(line, snprintf(line, SIZE, TAB "Material: \"%s\", \"\" {\n", materialName.c_str()));
        file->Write(line, snprintf(line, SIZE, TAB TAB "ShadingModel: \"%s\"\n", material->Specular ? "phong" : "lambert"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "Properties60: {\n"));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"AmbientColor\", \"Color\", \"A\", %.9g, %.9g, %.9g\n", material->AmbientColor.x, material->AmbientColor.y, material->AmbientColor.z));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"DiffuseColor\", \"Color\", \"A\", %.9g, %.9g, %.9g\n", material->DiffuseColor.x, material->DiffuseColor.y, material->DiffuseColor.z));
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"EmissiveColor\", \"Color\", \"A\", %.9g, %.9g, %.9g\n", material->EmissiveColor.x, material->EmissiveColor.y, material->EmissiveColor.z));
        if (material->Specular)
        {
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"SpecularColor\", \"Color\", \"A\", %.9g, %.9g, %.9g\n", material->SpecularColor.x, material->SpecularColor.y, material->SpecularColor.z));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"ShininessExponent\", \"Number\", \"A\", %.9g\n", material->SpecularHighlight));
        }
        file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"TransparencyFactor\", \"Number\", \"A\", %.9g\n", material->Opacity));
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
        file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#endif
    }

    for (size_t i = 0; i < material->Textures.size(); ++i)
    {
        xxTexturePtr const& texture = material->Textures[i];
        if (texture == nullptr)
            continue;
        std::string textureName = "Texture::" + xxFile::GetName(texture->Name.c_str());
#if (FBXVersion >= 7000)
        size_t textureHash = size_t(texture.get());
        if (std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureHash && std::get<1>(tuple) == materialHash; }) != connections.end())
            continue;
        bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureHash; }) != connections.end();
        connections.emplace_back(textureHash, materialHash, "Diffuse");
#else
        if (std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureName && std::get<1>(tuple) == materialName; }) != connections.end())
            continue;
        bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureName; }) != connections.end();
        connections.emplace_back(textureName, materialName, "Diffuse");
#endif
        if (found == false)
        {
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB "Texture: %zd, \"%s\", \"\" {\n", textureHash, textureName.c_str()));
            file->Write(line, snprintf(line, SIZE, TAB TAB "Properties70: {\n"));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"WrapModeU\", \"enum\", \"\", \"\", %d\n", texture->ClampU ? 0 : 1));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"WrapModeV\", \"enum\", \"\", \"\", %d\n", texture->ClampV ? 0 : 1));
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#else
            file->Write(line, snprintf(line, SIZE, TAB "Texture: \"%s\", \"\" {\n", textureName.c_str()));
            file->Write(line, snprintf(line, SIZE, TAB TAB "Properties60: {\n"));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"WrapModeU\", \"enum\", \"\", %d\n", texture->ClampU ? 0 : 1));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"WrapModeV\", \"enum\", \"\", %d\n", texture->ClampV ? 0 : 1));
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif
            file->Write(line, snprintf(line, SIZE, TAB TAB "RelativeFilename: \"%s\"\n", texture->Name.c_str()));
            file->Write(line, snprintf(line, SIZE, TAB "}\n"));
        }
    }
}
//------------------------------------------------------------------------------
static void fbx_skinning(xxFile* file, char* line, xxNodePtr const& node, Connections& connections)
{
    xxMeshPtr const& mesh = node->Mesh;

    // Skinning
    if (mesh == nullptr || mesh->Skinning == false || node->Bones.empty())
        return;

#if (FBXVersion >= 7000)
    // NodeAttribute
    for (size_t bone = 0; bone < node->Bones.size(); ++bone)
    {
        auto const& boneData = node->Bones[bone];
        xxNodePtr boneNode = boneData.bone.lock();
        while (boneNode)
        {
            size_t modelHash = size_t(boneNode.get());
            size_t attributeHash = modelHash + HASH_NODE_ATTRIBUTE;
            if (std::find_if(connections.begin(), connections.end(), [&](auto& tuple) { return std::get<0>(tuple) == attributeHash && std::get<1>(tuple) == modelHash; }) == connections.end())
            {
                connections.emplace_back(attributeHash, modelHash, "");

                file->Write(line, snprintf(line, SIZE, TAB "NodeAttribute: %zd, \"NodeAttribute::\", \"LimbNode\" {\n", attributeHash));
                file->Write(line, snprintf(line, SIZE, TAB TAB "TypeFlags: \"%s\"\n", "Skeleton"));
                file->Write(line, snprintf(line, SIZE, TAB "}\n"));
            }
            boneNode = boneNode->GetParent();
        }
    }

    // Deformer::Skin
    size_t geometryHash = size_t(mesh.get());
    size_t skinHash = geometryHash + HASH_SKIN;
    connections.emplace_back(skinHash, geometryHash, "");
    file->Write(line, snprintf(line, SIZE, TAB "Deformer: %zd, \"Deformer::\", \"Skin\" {\n", skinHash));
    file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#else
    // Deformer::Skin
    std::string modelName = "Model::" + node->Name;
    std::string skinName = "Deformer::Skin " + mesh->Name;
    connections.emplace_back(skinName, modelName, "");
    file->Write(line, snprintf(line, SIZE, TAB "Deformer: \"%s\", \"Skin\" {\n", skinName.c_str()));
    file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#endif

    // SubDeformer::Cluster
    for (size_t bone = 0; bone < node->Bones.size(); ++bone)
    {
        std::vector<std::pair<int, float>> clusters;

        auto boneIndices = mesh->GetBoneIndices();
        auto boneWeights = mesh->GetBoneWeight();
        for (int vertexIndex = 0; vertexIndex < mesh->VertexCount; ++vertexIndex)
        {
            xxVector3 weights3 = (*boneWeights++);
            uint32_t indices32 = (*boneIndices++);

            xxVector4 weights = { weights3.x, weights3.y, weights3.z, 1.0f - weights3.x - weights3.y - weights3.z };
            unsigned char indices[4];
            memcpy(indices, &indices32, 4);

            for (int i = 0; i < 4; ++i)
            {
                if (indices[i] == bone && weights[i] > FLT_EPSILON)
                {
                    clusters.emplace_back(vertexIndex, weights[i]);
                }
            }
        }

        auto const& boneData = node->Bones[bone];
        xxNodePtr boneNode = boneData.bone.lock();
        if (clusters.empty() == false && boneNode)
        {
            xxMatrix4 transform = node->WorldMatrix;
            xxMatrix4 transformLink = boneNode->WorldMatrix;

#if HAVE_RESET_SKINNING_MATRIX
            xxNodePtr const& parent = node->GetParent();
            if (parent)
            {
                transform = parent->WorldMatrix;
            }
#endif

#if (FBXVersion >= 7000)
            size_t clusterHash = skinHash + 1 + bone;
            size_t boneHash = size_t(boneNode.get());
            connections.emplace_back(clusterHash, skinHash, "");
            connections.emplace_back(boneHash, clusterHash, "");
            file->Write(line, snprintf(line, SIZE, TAB "Deformer: %zd, \"SubDeformer::\", \"Cluster\" {\n", clusterHash));
#else
            std::string clusterName = "SubDeformer::Cluster " + mesh->Name + '@' + boneNode->Name;
            std::string nodeName = "Model::" + boneNode->Name;
            connections.emplace_back(clusterName, skinName, "");
            connections.emplace_back(nodeName, clusterName, "");
            file->Write(line, snprintf(line, SIZE, TAB "Deformer: \"%s\", \"Cluster\" {\n", clusterName.c_str()));
#endif

            // Indexes
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "Indexes: *%zd {\n", clusters.size()));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
            file->Write(line, snprintf(line, SIZE, TAB TAB "Indexes: "));
#endif
            for (auto it = clusters.begin(); it != clusters.end(); ++it)
            {
                file->Write(line, snprintf(line, SIZE, "%s%d", it == clusters.begin() ? "" : ",", (*it).first));
            }
            file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif

            // Weights
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "Weights: *%zd {\n", clusters.size()));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
            file->Write(line, snprintf(line, SIZE, TAB TAB "Weights: "));
#endif
            for (auto it = clusters.begin(); it != clusters.end(); ++it)
            {
                file->Write(line, snprintf(line, SIZE, "%s%.9g", it == clusters.begin() ? "" : ",", (*it).second));
            }
            file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif

            // Transform
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "Transform: *%d {\n", 16));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
            file->Write(line, snprintf(line, SIZE, TAB TAB "Transform: "));
#endif
            file->Write(line, snprintf(line, SIZE, "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",
                        transform.m11, transform.m12, transform.m13, transform.m14,
                        transform.m21, transform.m22, transform.m23, transform.m24,
                        transform.m31, transform.m32, transform.m33, transform.m34,
                        transform.m41, transform.m42, transform.m43, transform.m44));
            file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif

            // TransformLink
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "TransformLink: *%d {\n", 16));
            file->Write(line, snprintf(line, SIZE, TAB TAB TAB "a: "));
#else
            file->Write(line, snprintf(line, SIZE, TAB TAB "TransformLink: "));
#endif
            file->Write(line, snprintf(line, SIZE, "%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g,%.9g",
                        transformLink.m11, transformLink.m12, transformLink.m13, transformLink.m14,
                        transformLink.m21, transformLink.m22, transformLink.m23, transformLink.m24,
                        transformLink.m31, transformLink.m32, transformLink.m33, transformLink.m34,
                        transformLink.m41, transformLink.m42, transformLink.m43, transformLink.m44));
            file->Write(line, snprintf(line, SIZE, "\n"));
#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
#endif

            file->Write(line, snprintf(line, SIZE, TAB "}\n"));
        }
    }
}
//------------------------------------------------------------------------------
static void fbx_transform(xxFile* file, char* line, xxNodePtr const& node)
{
    auto translate = node->GetTranslate();
    auto rotate = node->GetRotate().EularAngles();
    auto scale = node->GetScale();

#if HAVE_RESET_SKINNING_MATRIX
    if (node->Mesh && node->Mesh->Skinning && node->Bones.empty() == false)
    {
        translate = xxVector3::ZERO;
        rotate = xxVector3::ZERO;
        scale = 1.0f;
    }
#endif

#if (FBXVersion >= 7000)
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\", %.9g, %.9g, %.9g\n", translate.x, translate.y, translate.z));
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A\", %.9g, %.9g, %.9g\n", rotate.x, rotate.y, rotate.z));
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "P: \"Lcl Scaling\", \"Lcl Scaling\", \"\", \"A\", %.9g, %.9g, %.9g\n", scale, scale, scale));
#else
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"Lcl Translation\", \"Lcl Translation\", \"A\", %.9g, %.9g, %.9g\n", translate.x, translate.y, translate.z));
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"Lcl Rotation\", \"Lcl Rotation\", \"A\", %.9g, %.9g, %.9g\n", rotate.x, rotate.y, rotate.z));
    file->Write(line, snprintf(line, SIZE, TAB TAB TAB "Property: \"Lcl Scaling\", \"Lcl Scaling\", \"A\", %.9g, %.9g, %.9g\n", scale, scale, scale));
#endif
}
//------------------------------------------------------------------------------
bool ExportFilmbox::Save(char const* path, xxNodePtr const& root)
{
    xxFile* file = xxFile::Save(path);
    if (file == nullptr)
        return false;

    char line[SIZE] = {};
    Connections connections;

    // FBXHeaderExtension
    file->Write(line, snprintf(line, SIZE, "FBXHeaderExtension: {\n"));
    file->Write(line, snprintf(line, SIZE, TAB "FBXHeaderVersion: %d\n", 1003));
    file->Write(line, snprintf(line, SIZE, TAB "FBXVersion: %d\n", FBXVersion));
    file->Write(line, snprintf(line, SIZE, "}\n"));

    // Objects
    file->Write(line, snprintf(line, SIZE, "Objects: {\n"));
    Node::Traversal(root, [&](xxNodePtr const& node)
    {
        xxMeshPtr const& mesh = node->Mesh;
        xxNodePtr const& parent = node->GetParent();

        std::string modelName = "Model::" + node->Name;
#if (FBXVersion >= 7000)
        size_t parentHash = (root == parent) ? 0 : size_t(parent.get());
        size_t modelHash = size_t(node.get());
        connections.emplace_back(modelHash, parentHash, "");
        file->Write(line, snprintf(line, SIZE, TAB "Model: %zd, \"%s\", \"%s\" {\n", modelHash, modelName.c_str(), mesh ? "Mesh" : "LimbNode"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "Properties70: {\n"));
        fbx_transform(file, line, node);
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
        file->Write(line, snprintf(line, SIZE, TAB "}\n"));
        fbx_geometry(file, line, node, connections);
#else
        std::string parentName = (root == parent) ? "Model::Scene" : ("Model::" + parent->Name);
        connections.emplace_back(modelName, parentName, "");
        file->Write(line, snprintf(line, SIZE, TAB "Model: \"%s\", \"%s\" {\n", modelName.c_str(), mesh ? "Mesh" : "LimbNode"));
        file->Write(line, snprintf(line, SIZE, TAB TAB "Properties60: {\n"));
        fbx_transform(file, line, node);
        file->Write(line, snprintf(line, SIZE, TAB TAB "}\n"));
        fbx_geometry(file, line, node, connections);
        file->Write(line, snprintf(line, SIZE, TAB "}\n"));
#endif
        fbx_material(file, line, node, connections);
        fbx_skinning(file, line, node, connections);
        return true;
    });
    file->Write(line, snprintf(line, SIZE, "}\n"));

    // Connections
    if (connections.empty() == false)
    {
        file->Write(line, snprintf(line, SIZE, "Connections: {\n"));
        for (auto const& [left, right, extra] : connections)
        {
#if (FBXVersion >= 7000)
            if (extra.empty())
            {
                file->Write(line, snprintf(line, SIZE, TAB "C: \"OO\", %zd, %zd\n", left, right));
            }
            else
            {
                file->Write(line, snprintf(line, SIZE, TAB "C: \"OP\", %zd, %zd, \"%s\"\n", left, right, extra.c_str()));
            }
#else
            if (extra.empty())
            {
                file->Write(line, snprintf(line, SIZE, TAB "Connect: \"OO\", \"%s\", \"%s\"\n", left.c_str(), right.c_str()));
            }
            else
            {
                file->Write(line, snprintf(line, SIZE, TAB "Connect: \"OP\", \"%s\", \"%s\", \"%s\"\n", left.c_str(), right.c_str(), extra.c_str()));
            }
#endif
        }
        file->Write(line, snprintf(line, SIZE, "}\n"));
    }

    delete file;
    return true;
}
//==============================================================================
