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

#define TAG "ExportFilmbox"
#define TAB "\t"

//==============================================================================
bool ExportFilmbox::Save(char const* path, xxNodePtr const& root)
{
    xxFile* file = xxFile::Save(path);
    if (file == nullptr)
        return false;

    char line[256] = {};
    std::vector<std::pair<std::string, std::string>> connections;

    // FBXHeaderExtension
    file->Write(line, snprintf(line, 256, "FBXHeaderExtension: {\n"));
    file->Write(line, snprintf(line, 256, TAB "FBXHeaderVersion: %d\n", 1003));
    file->Write(line, snprintf(line, 256, TAB "FBXVersion: %d\n", 6000));
    file->Write(line, snprintf(line, 256, "}\n"));

    // Objects
    file->Write(line, snprintf(line, 256, "Objects: {\n"));
    Node::Traversal(root, [&](xxNodePtr const& node)
    {
        xxMaterialPtr const& material = node->Material;
        xxMeshPtr const& mesh = node->Mesh;

        // Material
        if (material && mesh)
        {
            std::string modelName = "Model::" + mesh->Name;
            std::string materialName = "Material::" + material->Name;
            if (std::find_if(connections.begin(), connections.end(), [materialName](auto const& pair){ return pair.first == materialName; }) == connections.end())
            {
                file->Write(line, snprintf(line, 256, TAB "Material: \"%s\", \"\" {\n", materialName.c_str()));
                file->Write(line, snprintf(line, 256, TAB TAB "ShadingModel: \"%s\"\n", material->Specular ? "phong" : "lambert"));
                file->Write(line, snprintf(line, 256, TAB TAB "Properties60: {\n"));
//              file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"ShadingModel\", \"KString\", \"\", \"%s\"\n", material->Specular ? "Phong" : "Lambert"));
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
            }
            connections.emplace_back(materialName, modelName);

            for (size_t i = 0; i < material->Textures.size(); ++i)
            {
                xxTexturePtr const& texture = material->Textures[i];
                if (texture == nullptr)
                    continue;
                std::string textureName = "Texture::" + xxFile::GetName(texture->Name.c_str());
                if (std::find_if(connections.begin(), connections.end(), [textureName](auto const& pair){ return pair.first == textureName; }) == connections.end())
                {
                    std::string modelName = "Model::" + mesh->Name;
                    file->Write(line, snprintf(line, 256, TAB "Texture: \"%s\", \"\" {\n", textureName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "TextureName: \"%s\"\n", textureName.c_str()));
//                  file->Write(line, snprintf(line, 256, TAB TAB "Properties60: {\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"UseMaterial\", \"bool\", \"\", 1\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
//                  file->Write(line, snprintf(line, 256, TAB TAB "FileName: \"%s\"\n", texture->Name.c_str()));
                    file->Write(line, snprintf(line, 256, TAB TAB "RelativeFilename: \"%s\"\n", texture->Name.c_str()));
                    file->Write(line, snprintf(line, 256, TAB "}\n"));
                }
                connections.emplace_back(textureName, modelName);
            }
        }

        // Mesh
        if (mesh)
        {
            std::string modelName = "Model::" + mesh->Name;
            file->Write(line, snprintf(line, 256, TAB "Model: \"%s\", \"Mesh\" {\n", modelName.c_str()));

            // Vertices
            if (mesh->VertexCount)
            {
                file->Write(line, snprintf(line, 256, TAB TAB "Vertices: "));
                for (auto vertices = mesh->GetPosition(); vertices != vertices.end(); ++vertices)
                {
                    xxVector3 vertex = (*vertices);
                    file->Write(line, snprintf(line, 256, "%s%g,%g,%g",
                                               vertices == vertices.begin() ? "" : ",",
                                               vertex.x, vertex.y, vertex.z));
                }
                file->Write(line, snprintf(line, 256, "\n"));
            }

            // PolygonVertexIndex
            if (mesh->IndexCount)
            {
                file->Write(line, snprintf(line, 256, TAB TAB "PolygonVertexIndex: "));
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
            }

            // LayerElementNormal
            if (mesh->NormalCount)
            {
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementNormal : {\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"ByVertex\"\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "Normals: "));
                for (auto normals = mesh->GetNormal(); normals != normals.end(); ++normals)
                {
                    xxVector3 normal = Mesh::NormalDecode(*normals);
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
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementUV : {\n"));
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

            // LayerElementTexture
            if (mesh->TextureCount)
            {
                file->Write(line, snprintf(line, 256, TAB TAB "LayerElementTexture : {\n"));
                file->Write(line, snprintf(line, 256, TAB TAB TAB "MappingInformationType: \"AllSame\"\n"));
                file->Write(line, snprintf(line, 256, TAB TAB "}\n"));
            }

            file->Write(line, snprintf(line, 256, TAB "}\n"));

            // Skinning
            if (mesh->Skinning && node->Bones.empty() == false)
            {
                // Deformer::Skin
                std::string skinName = "Deformer::Skin " + mesh->Name;
                connections.emplace_back(skinName, modelName);
                file->Write(line, snprintf(line, 256, TAB "Deformer: \"%s\", \"Skin\" {\n", skinName.c_str()));
                file->Write(line, snprintf(line, 256, TAB "}\n"));

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

                        std::string clusterName = "SubDeformer::Cluster " + mesh->Name + '@' + boneNode->Name;
                        std::string nodeName = "Model::" + boneNode->Name;
                        connections.emplace_back(clusterName, skinName);
                        connections.emplace_back(nodeName, clusterName);
                        file->Write(line, snprintf(line, 256, TAB "Deformer: \"%s\", \"Cluster\" {\n", clusterName.c_str()));

                        // Indexes
                        file->Write(line, snprintf(line, 256, TAB TAB "Indexes: "));
                        for (auto it = clusters.begin(); it != clusters.end(); ++it)
                        {
                            file->Write(line, snprintf(line, 256, "%s%d", it == clusters.begin() ? "" : ",", (*it).first));
                        }
                        file->Write(line, snprintf(line, 256, "\n"));

                        // Weights
                        file->Write(line, snprintf(line, 256, TAB TAB "Weights: "));
                        for (auto it = clusters.begin(); it != clusters.end(); ++it)
                        {
                            file->Write(line, snprintf(line, 256, "%s%g", it == clusters.begin() ? "" : ",", (*it).second));
                        }
                        file->Write(line, snprintf(line, 256, "\n"));

                        // Transform
                        file->Write(line, snprintf(line, 256, TAB TAB "Transform: %g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n",
                                    transform.m11, transform.m12, transform.m13, transform.m14,
                                    transform.m21, transform.m22, transform.m23, transform.m24,
                                    transform.m31, transform.m32, transform.m33, transform.m34,
                                    transform.m41, transform.m42, transform.m43, transform.m44));

                        // TransformLink
                        file->Write(line, snprintf(line, 256, TAB TAB "TransformLink: %g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n",
                                    transformLink.m11, transformLink.m12, transformLink.m13, transformLink.m14,
                                    transformLink.m21, transformLink.m22, transformLink.m23, transformLink.m24,
                                    transformLink.m31, transformLink.m32, transformLink.m33, transformLink.m34,
                                    transformLink.m41, transformLink.m42, transformLink.m43, transformLink.m44));

                        file->Write(line, snprintf(line, 256, TAB "}\n"));
                    }
                }
            }
        }

        // Node
        if (mesh == nullptr && node != root)
        {
            std::string parentName = "Null";
            xxNodePtr const& parent = node->GetParent();
            if (parent)
            {
                parentName = "Model::" + parent->Name;
            }

            std::string nodeName = "Model::" + node->Name;
            connections.emplace_back(nodeName, parentName);
            file->Write(line, snprintf(line, 256, TAB "Model: \"%s\", \"LimbNode\" {\n", nodeName.c_str()));
            file->Write(line, snprintf(line, 256, TAB TAB "Properties60: {\n"));

            auto translate = node->GetTranslate();
            auto rotate = node->GetRotate().EularAngles();
            auto scale = node->GetScale();

            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Translation\", \"Lcl Translation\", \"A\", %g, %g, %g\n", translate.x, translate.y, translate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Rotation\", \"Lcl Rotation\", \"A\", %g, %g, %g\n", rotate.x, rotate.y, rotate.z));
            file->Write(line, snprintf(line, 256, TAB TAB TAB "Property: \"Lcl Scaling\", \"Lcl Scaling\", \"A\", %g, %g, %g\n", scale, scale, scale));

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
        for (auto const& [name, parent] : connections)
        {
            file->Write(line, snprintf(line, 256, TAB "Connect: \"OO\", \"%s\", \"%s\"\n", name.c_str(), parent.c_str()));
        }
        file->Write(line, snprintf(line, 256, "}\n"));
    }

    delete file;
    return true;
}
//==============================================================================
