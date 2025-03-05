//==============================================================================
// Minamoto : ExportFilmbox Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxNode.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include "ExportFilmbox.h"

//==============================================================================
bool ExportFilmbox::Save(char const* path, xxNodePtr const& node)
{
    xxFile* file = xxFile::Save(path);
    if (file == nullptr)
        return false;

    char line[256];

    // FBXHeaderExtension
    file->Write(line, snprintf(line, 256, "FBXHeaderExtension: {\n"));
    file->Write(line, snprintf(line, 256, "\tFBXHeaderVersion: %d\n", 1003));
    file->Write(line, snprintf(line, 256, "\tFBXVersion: %d\n", 6000));
    file->Write(line, snprintf(line, 256, "}\n"));

    // Objects
    file->Write(line, snprintf(line, 256, "Objects: {\n"));
    xxNode::Traversal(node, [&](xxNodePtr const& node)
    {
        xxMeshPtr const& mesh = node->Mesh;
        if (mesh)
        {
            file->Write(line, snprintf(line, 256, "\tModel: \"%s\", \"Mesh\" {\n", mesh->Name.c_str()));

            // Vertices
            auto vertices = mesh->GetPosition();
            while (vertices != vertices.end())
            {
                bool first = (vertices == vertices.begin());
                bool last = (vertices + 1 == vertices.end());
                xxVector3 vertex = (*vertices++);
                file->Write(line, snprintf(line, 256, "\t\t%s%f,%f,%f%s\n",
                                           first ? "Vertices: " : "          ",
                                           vertex.x, vertex.y, vertex.z,
                                           last ? "" : ","));
            }

            // PolygonVertexIndex
            for (int i = 0; i < mesh->IndexCount; i += 3)
            {
                bool first = (i == 0);
                bool last = (i >= mesh->IndexCount - 3);
                int i0 = mesh->GetIndex(i + 0);
                int i1 = mesh->GetIndex(i + 1);
                int i2 = mesh->GetIndex(i + 2);
                file->Write(line, snprintf(line, 256, "\t\t%s%d,%d,-%d%s\n",
                                           first ? "PolygonVertexIndex: " : "                    ",
                                           i0, i1, i2,
                                           last ? "" : ","));
            }

            // LayerElementNormal
            auto normals = mesh->GetNormal();
            if (normals != normals.end())
            {
                file->Write(line, snprintf(line, 256, "\t\tLayerElementNormal : {\n"));
                file->Write(line, snprintf(line, 256, "\t\t\tMappingInformationType: \"ByVertex\"\n"));
                while (normals != normals.end())
                {
                    bool first = (normals == normals.begin());
                    bool last = (normals + 1 == normals.end());
                    xxVector3 normal = Mesh::NormalDecode(*normals++);
                    file->Write(line, snprintf(line, 256, "\t\t\t%s%f,%f,%f%s\n",
                                               first ? "Normals: " : "         ",
                                               normal.x, normal.y, normal.z,
                                               last ? "" : ","));
                }
                file->Write(line, snprintf(line, 256, "\t\t}\n"));
            }

            // LayerElementUV
            auto textures = mesh->GetTexture();
            if (textures != textures.end())
            {
                file->Write(line, snprintf(line, 256, "\t\tLayerElementUV : {\n"));
                file->Write(line, snprintf(line, 256, "\t\t\tMappingInformationType: \"ByVertex\"\n"));
                while (textures != textures.end())
                {
                    bool first = (textures == textures.begin());
                    bool last = (textures + 1 == textures.end());
                    xxVector2 texture = (*textures++);
                    file->Write(line, snprintf(line, 256, "\t\t\t%s%f,%f%s\n",
                                               first ? "UV: " : "    ",
                                               texture.u, texture.v,
                                               last ? "" : ","));
                }
                file->Write(line, snprintf(line, 256, "\t\t}\n"));
            }

            file->Write(line, snprintf(line, 256, "\t}\n"));
        }
        return true;
    });
    file->Write(line, snprintf(line, 256, "}\n"));

    delete file;
    return true;
}
//==============================================================================
