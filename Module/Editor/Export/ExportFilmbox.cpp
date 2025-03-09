//==============================================================================
// Minamoto : ExportFilmbox Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
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

//==============================================================================
bool ExportFilmbox::Save(char const* path, xxNodePtr const& root)
{
    xxFile* file = xxFile::Save(path);
    if (file == nullptr)
        return false;

    static xxMatrix3 swapYZ3 = { xxVector3::X, xxVector3::Z, -xxVector3::Y };
    static xxMatrix4 swapYZ4 = { xxVector4::X, xxVector4::Z, -xxVector4::Y, xxVector4::W };
    static xxMatrix3 invSwapYZ3 = swapYZ3.Transpose();

    char line[256] = {};
#if (FBXVersion >= 7000)
    int geometryCount = 0;
    int materialCount = 0;
    int modelCount = 0;
    int textureCount = 0;
//  int videoCount = 0;
    std::vector<std::tuple<size_t, size_t, std::string>> connections;
#else
    std::vector<std::pair<std::string, std::string>> connections;
#endif

    // FBXHeaderExtension
    file->Write(line, snprintf(line, 256, "FBXHeaderExtension: {\n"));
    file->Write(line, snprintf(line, 256, TAB "FBXHeaderVersion: %d\n", 1003));
    file->Write(line, snprintf(line, 256, TAB "FBXVersion: %d\n", FBXVersion));
    file->Write(line, snprintf(line, 256, "}\n"));

    // Objects
    file->Write(line, snprintf(line, 256, "Objects: {\n"));
    Node::Traversal(root, [&](xxNodePtr const& node)
    {
        xxMaterialPtr const& material = node->Material;
        xxMeshPtr const& mesh = node->Mesh;
        xxNodePtr const& parent = node->GetParent();

        // Material
        if (material && mesh)
        {
            std::string modelName = "Model::" + mesh->Name;
            std::string materialName = "Material::" + material->Name;
#if (FBXVersion >= 7000)
            size_t modelHash = size_t(mesh.get());
            size_t materialHash = size_t(material.get());
            bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == materialHash; }) != connections.end();
            connections.emplace_back(materialHash, modelHash, "");
#else
            bool found = std::find_if(connections.begin(), connections.end(), [&](auto& pair){ return pair.first == materialName; }) != connections.end();
            connections.emplace_back(materialName, modelName);
#endif
            if (found == false)
            {
#if (FBXVersion >= 7000)
                materialCount++;
                file->Write(line, snprintf(line, 256, TAB "Material: %zd, \"%s\", \"\" {\n", materialHash, materialName.c_str()));
                file->Write(line, snprintf(line, 256, TAB TAB "ShadingModel: \"%s\"\n", material->Specular ? "phong" : "lambert"));
                file->Write(line, snprintf(line, 256, TAB TAB "Properties70: {\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"AmbientColor\", \"Color\", \"\", \"A\", %g, %g, %g\n", material->AmbientColor.x, material->AmbientColor.y, material->AmbientColor.z));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"DiffuseColor\", \"Color\", \"\", \"A\", %g, %g, %g\n", material->DiffuseColor.x, material->DiffuseColor.y, material->DiffuseColor.z));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"EmissiveColor\", \"Color\", \"\", \"A\", %g, %g, %g\n", material->EmissiveColor.x, material->EmissiveColor.y, material->EmissiveColor.z));
                if (material->Specular)
                {
                    file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"SpecularColor\", \"Color\", \"\", \"A\", %g, %g, %g\n", material->SpecularColor.x, material->SpecularColor.y, material->SpecularColor.z));
                    file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"ShininessExponent\", \"Number\", \"\", \"A\", %g\n", material->SpecularHighlight));
                }
                file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"TransparencyFactor\", \"Number\", \"\", \"A\", %g\n", material->Opacity));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
                file->Write(line, snprintf(line, 256, TAB "}\n"));
#else
                file->Write(line, snprintf(line, 256, TAB "Material: \"%s\", \"\" {\n", materialName.c_str()));
                file->Write(line, snprintf(line, 256, TAB TAB "ShadingModel: \"%s\"\n", material->Specular ? "phong" : "lambert"));
                file->Write(line, snprintf(line, 256, TAB TAB "Properties60: {\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"AmbientColor\", \"Color\", \"A\", %g, %g, %g\n", material->AmbientColor.x, material->AmbientColor.y, material->AmbientColor.z));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"DiffuseColor\", \"Color\", \"A\", %g, %g, %g\n", material->DiffuseColor.x, material->DiffuseColor.y, material->DiffuseColor.z));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"EmissiveColor\", \"Color\", \"A\", %g, %g, %g\n", material->EmissiveColor.x, material->EmissiveColor.y, material->EmissiveColor.z));
                if (material->Specular)
                {
                    file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"SpecularColor\", \"Color\", \"A\", %g, %g, %g\n", material->SpecularColor.x, material->SpecularColor.y, material->SpecularColor.z));
                    file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"ShininessExponent\", \"Number\", \"A\", %g\n", material->SpecularHighlight));
                }
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"TransparencyFactor\", \"Number\", \"A\", %g\n", material->Opacity));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
                file->Write(line, snprintf(line, 256, TAB "}\n"));
#endif
            }

            for (size_t i = 0; i < material->Textures.size(); ++i)
            {
                xxTexturePtr const& texture = material->Textures[i];
                if (texture == nullptr)
                    continue;
                std::string textureName = "Texture::" + xxFile::GetName(texture->Name.c_str());
#if (FBXVersion >= 7000)
                std::string videoName = "Video::" + xxFile::GetName(texture->Name.c_str());
                size_t textureHash = size_t(texture.get());
//              size_t videoHash = textureHash + 1;
                if (std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureHash && std::get<1>(tuple) == materialHash; }) != connections.end())
                {
                    continue;
                }
                bool found = std::find_if(connections.begin(), connections.end(), [&](auto& tuple){ return std::get<0>(tuple) == textureHash; }) != connections.end();
                connections.emplace_back(textureHash, materialHash, "DiffuseColor");
//              connections.emplace_back(videoHash, textureHash, "");
#else
                bool found = std::find_if(connections.begin(), connections.end(), [&](auto& pair){ return pair.first == textureName; }) != connections.end();
                connections.emplace_back(textureName, modelName);
#endif
                if (found == false)
                {
#if (FBXVersion >= 7000)
//                  videoCount++;
//                  file->Write(line, snprintf(line, 256, TAB "Video: %zd, \"%s\", \"Clip\" {\n", videoHash, videoName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Type: \"%s\"\n", "Clip"));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Properties70: {\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Path\", \"KString\", \"XRefUrl\", \"\", \"%s\"\n", texture->Name.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"RelPath\", \"KString\", \"XRefUrl\", \"\", \"%s\"\n", texture->Name.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Filename: \"%s\"\n", texture->Name.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "RelativeFilename: \"%s\"\n", texture->Name.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB "}\n"));
                    textureCount++;
                    file->Write(line, snprintf(line, 256, TAB "Texture: %zd, \"%s\", \"\" {\n", textureHash, textureName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Type: \"%s\"\n", "TextureVideoClip"));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Media: \"%s\"\n", videoName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Properties70: {\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"UVSet\", \"KString\", \"\", \"\", \"%s\"\n", "map1"));
//                  file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"UseMaterial\", \"bool\", \"\", \"\", %d\n", 1));
//                  file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#else
                    file->Write(line, snprintf(line, 256, TAB "Texture: \"%s\", \"\" {\n", textureName.c_str()));
#endif
//                  file->Write(line, snprintf(line, 256, TAB TAB "TextureName: \"%s\"\n", textureName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Filename: \"%s\"\n", texture->Name.c_str()));
                    file->Write(line, snprintf(line, 256, TAB TAB "RelativeFilename: \"%s\"\n", texture->Name.c_str()));
                    file->Write(line, snprintf(line, 256, TAB "}\n"));
                }
            }
        }

        // Mesh
        if (mesh)
        {
            std::string modelName = "Model::" + mesh->Name;
#if (FBXVersion >= 7000)
            size_t parentHash = (root == parent) ? 0 : size_t(parent.get());
            size_t modelHash = size_t(mesh.get());
            size_t geometryHash = modelHash + 1;
            connections.emplace_back(modelHash, parentHash, "");
            connections.emplace_back(geometryHash, modelHash, "");
            modelCount++;
            file->Write(line, snprintf(line, 256, TAB "Model: %zd, \"%s\", \"Mesh\" {\n", modelHash, modelName.c_str()));
            file->Write(line, snprintf(line, 256, TAB TAB "Properties70: {\n"));

#if 0
            auto translate = node->GetTranslate();
            auto rotate = node->GetRotate().EularAngles();
            auto scale = node->GetScale();

            if (parent == root)
            {
                translate = swapYZ3 * translate;
                rotate = (node->GetRotate() * swapYZ3).EularAngles();
            }

            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\", %g, %g, %g\n", translate.x, translate.y, translate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A\", %g, %g, %g\n", rotate.x, rotate.y, rotate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Scaling\", \"Lcl Scaling\", \"\", \"A\", %g, %g, %g\n", scale, scale, scale));
#endif

//          file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"currentUVSet\", \"KString\", \"\", \"U\", \"%s\"\n", "map1"));
            file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//          file->Write(line, snprintf(line, 256, TAB TAB "Shading: T\n"));
            file->Write(line, snprintf(line, 256, TAB "}\n"));
            geometryCount++;
            file->Write(line, snprintf(line, 256, TAB "Geometry: %zd, \"Geometry::\", \"Mesh\" {\n", geometryHash));
#else
            std::string parentName = (root == parent) ? "Model::Scene" : ("Model::" + parent->Name);
            connections.emplace_back(modelName, parentName);
            file->Write(line, snprintf(line, 256, TAB "Model: \"%s\", \"Mesh\" {\n", modelName.c_str()));
#endif

            // Vertices
            if (mesh->VertexCount)
            {
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "Vertices: *%d {\n", mesh->VertexCount));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                file->Write(line, snprintf(line, 256, TAB TAB "Vertices: "));
#endif
                for (auto vertices = mesh->GetPosition(); vertices != vertices.end(); ++vertices)
                {
                    xxVector3 vertex = (*vertices);
                    vertex = vertex * invSwapYZ3;
                    file->Write(line, snprintf(line, 256, "%s%g,%g,%g",
                                               vertices == vertices.begin() ? "" : ",",
                                               vertex.x, vertex.y, vertex.z));
                }
                file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif
            }

            // PolygonVertexIndex
            if (mesh->IndexCount)
            {
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "PolygonVertexIndex: *%d {\n", mesh->VertexCount));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                file->Write(line, snprintf(line, 256, TAB TAB "PolygonVertexIndex: "));
#endif
                for (int i = 0; i < mesh->IndexCount; i += 3)
                {
                    int i0 = mesh->GetIndex(i + 0);
                    int i1 = mesh->GetIndex(i + 1);
                    int i2 = mesh->GetIndex(i + 2);
                    file->Write(line, snprintf(line, 256, "%s%d,%d,%d",
                                               i == 0 ? "" : ",",
                                               i0, i1, ~i2));
                }
                file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif
            }

            // LayerElementNormal
            if (mesh->NormalCount)
            {
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementNormal: 0 {\n"));
#else
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementNormal: {\n"));
#endif
                file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Normals: "));
                for (auto normals = mesh->GetNormal(); normals != normals.end(); ++normals)
                {
                    xxVector3 normal = Mesh::NormalDecode(*normals);
                    normal = normal * invSwapYZ3;
                    file->Write(line, snprintf(line, 256, "%s%g,%g,%g",
                                               normals == normals.begin() ? "" : ",",
                                               normal.x, normal.y, normal.z));
                }
                file->Write(line, snprintf(line, 256, "\n"));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
            }

            // LayerElementUV
            if (mesh->TextureCount)
            {
#if (FBXVersion >= 7000)
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementUV: 0 {\n"));
#else
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementUV: {\n"));
#endif
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "Name: \"%s\"\n", "map1"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "UV: "));
                for (auto textures = mesh->GetTexture(); textures != textures.end(); ++textures)
                {
                    xxVector2 texture = (*textures);
                    file->Write(line, snprintf(line, 256, "%s%g,%g",
                                               textures == textures.begin() ? "" : ",",
                                               texture.x, texture.y));
                }
                file->Write(line, snprintf(line, 256, "\n"));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
            }

#if (FBXVersion >= 7000)
//          // LayerElementMaterial
//          if (node->Material)
//          {
//              file->Write(line, snprintf(line, 256, TAB TAB "LayerElementMaterial: 0 {\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"%s\"\n", "AllSame"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "ReferenceInformationType: \"%s\"\n", "IndexToDirect"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "Materials: *%d {\n", 1));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "a: 0\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "}\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//          }

//          file->Write(line, snprintf(line, 256, TAB TAB "Layer: 0 {\n"));
//          if (mesh->NormalCount)
//          {
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "LayerElement: {\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "Type: \"%s\"\n", "LayerElementNormal"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "TypedIndex: %d\n", 0));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "}\n"));
//          }
//          if (mesh->TextureCount)
//          {
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "LayerElement: {\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "Type: \"%s\"\n", "LayerElementUV"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "TypedIndex: %d\n", 0));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "}\n"));
//          }
//          if (node->Material)
//          {
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "LayerElement: {\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "Type: \"%s\"\n", "LayerElementMaterial"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB TAB "TypedIndex: %d\n", 0));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "}\n"));
//          }
//          file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#else
            // LayerElementTexture
            if (mesh->TextureCount)
            {
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementTexture: {\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"AllSame\"\n"));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
            }
#endif

            file->Write(line, snprintf(line, 256, TAB "}\n"));

            // Skinning
            if (mesh->Skinning && node->Bones.empty() == false)
            {
                // Deformer::Skin
#if (FBXVersion >= 7000)
                size_t skinHash = modelHash + 2;
                connections.emplace_back(skinHash, geometryHash, "");
                file->Write(line, snprintf(line, 256, TAB "Deformer: %zd, \"Deformer::\", \"Skin\" {\n", skinHash));
                file->Write(line, snprintf(line, 256, TAB "}\n"));
#else
                std::string skinName = "Deformer::Skin " + mesh->Name;
                connections.emplace_back(skinName, modelName);
                file->Write(line, snprintf(line, 256, TAB "Deformer: \"%s\", \"Skin\" {\n", skinName.c_str()));
                file->Write(line, snprintf(line, 256, TAB "}\n"));
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
                        xxMatrix4 transform = boneData.skinMatrix;
                        xxMatrix4 transformLink = transform.Inverse();

                        transform = transform * swapYZ4;
                        transformLink = transform.Inverse();

#if (FBXVersion >= 7000)
                        size_t clusterHash = modelHash + 3 + bone;
                        size_t nodeHash = size_t(boneNode.get());
                        connections.emplace_back(clusterHash, skinHash, "");
                        connections.emplace_back(nodeHash, clusterHash, "");
                        file->Write(line, snprintf(line, 256, TAB "Deformer: %zd, \"SubDeformer::\", \"Cluster\" {\n", clusterHash));
#else
                        std::string clusterName = "SubDeformer::Cluster " + mesh->Name + '@' + boneNode->Name;
                        std::string nodeName = "Model::" + boneNode->Name;
                        connections.emplace_back(clusterName, skinName);
                        connections.emplace_back(nodeName, clusterName);
                        file->Write(line, snprintf(line, 256, TAB "Deformer: \"%s\", \"Cluster\" {\n", clusterName.c_str()));
#endif

                        // Indexes
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "Indexes: *%zd {\n", clusters.size()));
                        file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                        file->Write(line, snprintf(line, 256, TAB TAB "Indexes: "));
#endif
                        for (auto it = clusters.begin(); it != clusters.end(); ++it)
                        {
                            file->Write(line, snprintf(line, 256, "%s%d", it == clusters.begin() ? "" : ",", (*it).first));
                        }
                        file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif

                        // Weights
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "Weights: *%zd {\n", clusters.size()));
                        file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                        file->Write(line, snprintf(line, 256, TAB TAB "Weights: "));
#endif
                        for (auto it = clusters.begin(); it != clusters.end(); ++it)
                        {
                            file->Write(line, snprintf(line, 256, "%s%g", it == clusters.begin() ? "" : ",", (*it).second));
                        }
                        file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif

                        // Transform
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "Transform: *%d {\n", 16));
                        file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                        file->Write(line, snprintf(line, 256, TAB TAB "Transform: "));
#endif
                        file->Write(line, snprintf(line, 256, "%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g",
                                    transform.m11, transform.m12, transform.m13, transform.m14,
                                    transform.m21, transform.m22, transform.m23, transform.m24,
                                    transform.m31, transform.m32, transform.m33, transform.m34,
                                    transform.m41, transform.m42, transform.m43, transform.m44));
                        file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif

                        // TransformLink
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "TransformLink: *%d {\n", 16));
                        file->Write(line, snprintf(line, 256, TAB TAB TAB "a: "));
#else
                        file->Write(line, snprintf(line, 256, TAB TAB "TransformLink: "));
#endif
                        file->Write(line, snprintf(line, 256, "%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g",
                                    transformLink.m11, transformLink.m12, transformLink.m13, transformLink.m14,
                                    transformLink.m21, transformLink.m22, transformLink.m23, transformLink.m24,
                                    transformLink.m31, transformLink.m32, transformLink.m33, transformLink.m34,
                                    transformLink.m41, transformLink.m42, transformLink.m43, transformLink.m44));
                        file->Write(line, snprintf(line, 256, "\n"));
#if (FBXVersion >= 7000)
                        file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
#endif

                        file->Write(line, snprintf(line, 256, TAB "}\n"));
                    }
                }
            }
        }

        // Node
        if (mesh == nullptr && parent && node != root)
        {
            std::string nodeName = "Model::" + node->Name;
#if (FBXVersion >= 7000)
            size_t parentHash = (root == parent) ? 0 : size_t(parent.get());
            size_t nodeHash = size_t(node.get());
            connections.emplace_back(nodeHash, parentHash, "");
            file->Write(line, snprintf(line, 256, TAB "Model: %zd, \"%s\", \"LimbNode\" {\n", nodeHash, nodeName.c_str()));
            file->Write(line, snprintf(line, 256, TAB TAB "Properties70: {\n"));
#else
            std::string parentName = (root == parent) ? "Model::Scene" : ("Model::" + parent->Name);
            connections.emplace_back(nodeName, parentName);
            file->Write(line, snprintf(line, 256, TAB "Model: \"%s\", \"LimbNode\" {\n", nodeName.c_str()));
            file->Write(line, snprintf(line, 256, TAB TAB "Properties60: {\n"));
#endif

            auto translate = node->GetTranslate();
            auto rotate = node->GetRotate().EularAngles();
            auto scale = node->GetScale();

            if (parent == root)
            {
                translate = translate * invSwapYZ3;
                rotate = (invSwapYZ3 * node->GetRotate()).EularAngles();
            }

#if (FBXVersion >= 7000)
            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Translation\", \"Lcl Translation\", \"\", \"A\", %g, %g, %g\n", translate.x, translate.y, translate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Rotation\", \"Lcl Rotation\", \"\", \"A\", %g, %g, %g\n", rotate.x, rotate.y, rotate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "P: \"Lcl Scaling\", \"Lcl Scaling\", \"\", \"A\", %g, %g, %g\n", scale, scale, scale));
#else
            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Translation\", \"Lcl Translation\", \"A\", %g, %g, %g\n", translate.x, translate.y, translate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Rotation\", \"Lcl Rotation\", \"A\", %g, %g, %g\n", rotate.x, rotate.y, rotate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Scaling\", \"Lcl Scaling\", \"A\", %g, %g, %g\n", scale, scale, scale));
#endif

            file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
            file->Write(line, snprintf(line, 256, TAB "}\n"));
        }
        return true;
    });
    file->Write(line, snprintf(line, 256, "}\n"));

    // Connections
    if (connections.empty() == false)
    {
        file->Write(line, snprintf(line, 256, "Connections: {\n"));
#if (FBXVersion >= 7000)
        for (auto const& [left, right, extra] : connections)
        {
            if (extra.empty())
            {
                file->Write(line, snprintf(line, 256, TAB "C: \"OO\", %zd, %zd\n", left, right));
            }
            else
            {
                file->Write(line, snprintf(line, 256, TAB "C: \"OP\", %zd, %zd, \"%s\"\n", left, right, extra.c_str()));
            }
        }
#else
        for (auto const& [left, right] : connections)
        {
            file->Write(line, snprintf(line, 256, TAB "Connect: \"OO\", \"%s\", \"%s\"\n", left.c_str(), right.c_str()));
        }
#endif
        file->Write(line, snprintf(line, 256, "}\n"));
    }

#if (FBXVersion >= 7000)
//  // Definitions
//  file->Write(line, snprintf(line, 256, "Definitions: {\n"));
//  file->Write(line, snprintf(line, 256, TAB "Count: %d\n", geometryCount + materialCount + modelCount + textureCount + videoCount));
//  if (geometryCount)
//  {
//      file->Write(line, snprintf(line, 256, TAB "ObjectType: \"%s\" {\n", "Geometry"));
//      file->Write(line, snprintf(line, 256, TAB TAB "Count: %d\n", geometryCount));
//      file->Write(line, snprintf(line, 256, TAB TAB "PropertyTemplate: \"%s\" {\n", "FbxMesh"));
//      file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//      file->Write(line, snprintf(line, 256, TAB "}\n"));
//  }
//  if (materialCount)
//  {
//      file->Write(line, snprintf(line, 256, TAB "ObjectType: \"%s\" {\n", "Material"));
//      file->Write(line, snprintf(line, 256, TAB TAB "Count: %d\n", materialCount));
//      file->Write(line, snprintf(line, 256, TAB TAB "PropertyTemplate: \"%s\" {\n", "FbxSurfacePhong"));
//      file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//      file->Write(line, snprintf(line, 256, TAB "}\n"));
//  }
//  if (modelCount)
//  {
//      file->Write(line, snprintf(line, 256, TAB "ObjectType: \"%s\" {\n", "Model"));
//      file->Write(line, snprintf(line, 256, TAB TAB "Count: %d\n", modelCount));
//      file->Write(line, snprintf(line, 256, TAB TAB "PropertyTemplate: \"%s\" {\n", "FbxNode"));
//      file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//      file->Write(line, snprintf(line, 256, TAB "}\n"));
//  }
//  if (textureCount)
//  {
//      file->Write(line, snprintf(line, 256, TAB "ObjectType: \"%s\" {\n", "Texture"));
//      file->Write(line, snprintf(line, 256, TAB TAB "Count: %d\n", textureCount));
//      file->Write(line, snprintf(line, 256, TAB TAB "PropertyTemplate: \"%s\" {\n", "FbxFileTexture"));
//      file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//      file->Write(line, snprintf(line, 256, TAB "}\n"));
//  }
//  if (videoCount)
//  {
//      file->Write(line, snprintf(line, 256, TAB "ObjectType: \"%s\" {\n", "Video"));
//      file->Write(line, snprintf(line, 256, TAB TAB "Count: %d\n", videoCount));
//      file->Write(line, snprintf(line, 256, TAB TAB "PropertyTemplate: \"%s\" {\n", "FbxVideo"));
//      file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//      file->Write(line, snprintf(line, 256, TAB "}\n"));
//  }
//  file->Write(line, snprintf(line, 256, "}\n"));
#endif

    delete file;
    return true;
}
//==============================================================================
