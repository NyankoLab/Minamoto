//==============================================================================
// Minamoto : ExportWavefront Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxNode.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include "ExportWavefront.h"

//==============================================================================
bool ExportWavefront::Save(char const* path, xxNodePtr const& node)
{
    xxFile* file = xxFile::Save(path);
    if (file == nullptr)
        return false;

    char line[256];
    file->Write(line, snprintf(line, 256, "mtllib %s.mtl\n", xxFile::GetName(path).c_str()));

    std::vector<xxVector3> vertices;
    std::vector<xxVector3> normals;
    std::vector<xxVector2> textures;
    std::vector<size_t> faceVertices;
    std::vector<size_t> faceNormals;
    std::vector<size_t> faceTextures;
    for (xxNodePtr const& child : (*node))
    {
        xxMaterialPtr const& material = child->Material;
        xxMeshPtr const& mesh = child->Mesh;
        if (mesh)
        {
            if (material)
            {
                file->Write(line, snprintf(line, 256, "usemtl %s\n", material->Name.c_str()));
            }
            file->Write(line, snprintf(line, 256, "g %s\n", mesh->Name.c_str()));
            file->Write(line, snprintf(line, 256, "s 1\n"));

            unsigned int count = mesh->IndexCount;
            if (count == 0)
                count = mesh->VertexCount;

            faceVertices.clear();
            faceNormals.clear();
            faceTextures.clear();
            if (true)
            {
                for (unsigned int i = 0; i < count; ++i)
                {
                    xxVector3 v = *(mesh->GetPosition() + i);
                    auto it = std::find(vertices.begin(), vertices.end(), v);
                    if (it == vertices.end())
                    {
                        faceVertices.push_back(vertices.size());
                        vertices.push_back(v);
                        file->Write(line, snprintf(line, 256, "v %f %f %f\n", v.x, v.z, -v.y));
                    }
                    else
                    {
                        faceVertices.push_back(std::distance(vertices.begin(), it));
                    }
                }
            }
            if (mesh->NormalCount)
            {
                for (unsigned int i = 0; i < count; ++i)
                {
                    xxVector3 n = Mesh::NormalDecode(*(mesh->GetNormal() + i));
                    auto it = std::find(normals.begin(), normals.end(), n);
                    if (it == normals.end())
                    {
                        faceNormals.push_back(normals.size());
                        normals.push_back(n);
                        file->Write(line, snprintf(line, 256, "vn %f %f %f\n", n.x, n.z, -n.y));
                    }
                    else
                    {
                        faceNormals.push_back(std::distance(normals.begin(), it));
                    }
                }
            }
            if (mesh->TextureCount)
            {
                for (unsigned int i = 0; i < count; ++i)
                {
                    xxVector2 t = *(mesh->GetTexture() + i);
                    auto it = std::find(textures.begin(), textures.end(), t);
                    if (it == textures.end())
                    {
                        faceTextures.push_back(textures.size());
                        textures.push_back(t);
                        file->Write(line, snprintf(line, 256, "vt %f %f\n", t.u, 1 - t.v));
                    }
                    else
                    {
                        faceTextures.push_back(std::distance(textures.begin(), it));
                    }
                }
            }

            for (unsigned int i = 0; i < count; ++i)
            {
                if (i % 3 == 0)
                {
                    snprintf(line, 256, "f ");
                }
                else
                {
                    snprintf(line, 256, "%s ", line);
                }

                snprintf(line, 256, "%s%zd", line, faceVertices[i] + 1);
                if (faceTextures.size() > i)
                {
                    snprintf(line, 256, "%s/", line);
                    snprintf(line, 256, "%s%zd", line, faceTextures[i] + 1);
                }
                else
                {
                    snprintf(line, 256, "%s/", line);
                }
                if (faceNormals.size() > i)
                {
                    snprintf(line, 256, "%s/", line);
                    snprintf(line, 256, "%s%zd", line, faceNormals[i] + 1);
                }

                if (i % 3 == 2)
                {
                    file->Write(line, snprintf(line, 256, "%s\n", line));
                }
            }
        }
    }

    delete file;
    return true;
}
//==============================================================================
