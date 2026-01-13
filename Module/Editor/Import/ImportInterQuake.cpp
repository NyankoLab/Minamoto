//==============================================================================
// Minamoto : ImportInterQuake Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <map>
#include <xxGraphicPlus/xxFile.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include "Utility/MeshTools.h"
#include "ImportWavefront.h"
#include "ImportInterQuake.h"

#define TAG "ImportInterQuake"

//==============================================================================
static void RemoveBreakline(char* text)
{
    if (char* slash = strrchr(text, '\r'))
        slash[0] = 0;
    if (char* slash = strrchr(text, '\n'))
        slash[0] = 0;
}
//------------------------------------------------------------------------------
static int ToInt(char const* text)
{
    return text ? atoi(text) : -1;
}
//------------------------------------------------------------------------------
static float ToFloat(char const* text)
{
    return text ? float(atof(text)) : 0.0f;
}
//------------------------------------------------------------------------------
xxNodePtr ImportInterQuake::Create(char const* iqe, std::function<void(xxNodePtr&&)> callback)
{
    FILE* file = fopen(iqe, "rb");
    if (file == nullptr)
        return xxNodePtr();

    // joint
    std::map<std::string, xxNodePtr> jointNodes;
    std::vector<std::string> jointNames;
    std::string jointParent;
    xxNodePtr jointNode;

    // mesh
    std::vector<xxVector3> vertices;
    std::vector<xxVector3> boneWeights;
    std::vector<uint32_t> boneIndices;
    std::vector<xxVector3> normals;
    std::vector<xxVector4> colors;
    std::vector<xxVector2> textures;
    std::vector<uint32_t> indices;
    std::string name;

    // material
    std::string mtllib = xxFile::GetPath(iqe, true) + xxFile::GetName(iqe) + ".mtl";
    auto materials = ImportWavefront::CreateMaterial(mtllib.c_str());
    ImportWavefront::WavefrontMaterial* material = nullptr;

    xxNodePtr root = xxNode::Create();
    root->Name = xxFile::GetName(iqe);
    root->Flags |= xxNode::UPDATE_NEED;

    auto finish = [&]()
    {
        if (vertices.empty() == false)
        {
            xxNodePtr child = xxNode::Create();
            child->Name = CheckDuplicateName(root, name);

            xxMeshPtr mesh = child->Mesh = MeshTools::CreateMesh(vertices, boneWeights, boneIndices, normals, colors, textures, indices);
            if (mesh)
            {
                mesh->Name = child->Name;
                if (mesh->Skinning)
                {
                    for (size_t i = 0; i < jointNames.size(); ++i)
                    {
                        xxVector4 bound = xxVector4::ZERO;
                        auto position = mesh->GetPosition();
                        auto weights = mesh->GetBoneWeight();
                        auto indices = mesh->GetBoneIndices();
                        while (position != position.end() && weights != weights.end() && indices != indices.end())
                        {
                            xxVector3 vertex = (*position++);
                            xxVector3 weight = (*weights++);
                            uint32_t index = (*indices++);
                            if ((((index >>  0) & 0xFF) == i && weight.x != 0.0f) ||
                                (((index >>  8) & 0xFF) == i && weight.y != 0.0f) ||
                                (((index >> 16) & 0xFF) == i && weight.z != 0.0f) ||
                                (((index >> 24) & 0xFF) == i && (weight.x + weight.y + weight.z) != 1.0f))
                            {
                                bound.BoundMerge(vertex);
                            }
                        }

                        std::string const& name = jointNames[i];
                        xxNodePtr const& to = jointNodes[name];
                        xxMatrix4 invWorldMatrix = to->WorldMatrix.Inverse();

                        xxNode::BoneData bone;
                        bone.bone = to;
                        bone.bound = bound.BoundTransform(invWorldMatrix);
                        bone.classSkinMatrix = invWorldMatrix;
                        bone.classBoneMatrix = {};
                        child->Bones.push_back(bone);
                    }
                    for (auto& data : child->Bones)
                    {
                        data.ResetPointer();
                    }
                }
            }
            if (material)
            {
                child->Material = material->output;
            }
            if (callback)
            {
                callback(std::move(child));
            }
            else
            {
                root->AttachChild(child);
            }
        }
        jointNames.clear();
        jointParent.clear();
        jointNode = nullptr;
        vertices.clear();
        boneWeights.clear();
        boneIndices.clear();
        normals.clear();
        colors.clear();
        textures.clear();
        indices.clear();
        name.clear();
        material = nullptr;
    };

    char line[256];
    while (fgets(line, 256, file))
    {
        RemoveBreakline(line);

        char* lasts;
        char const* delim = " \t";
        char const* statement = strtok_r(line, delim, &lasts);
        if (statement == nullptr || lasts == nullptr)
            continue;

        switch (xxHash(statement))
        {
        case xxHash("vp"):
            vertices.push_back(xxVector3::ZERO);
            vertices.back().x = ToFloat(strtok_r(nullptr, delim, &lasts));
            vertices.back().z = ToFloat(strtok_r(nullptr, delim, &lasts));
            vertices.back().y = -ToFloat(strtok_r(nullptr, delim, &lasts));
            break;
        case xxHash("vt"):
            textures.push_back(xxVector2::ZERO);
            textures.back().x = ToFloat(strtok_r(nullptr, delim, &lasts));
            textures.back().y = ToFloat(strtok_r(nullptr, delim, &lasts));
            break;
        case xxHash("vn"):
            normals.push_back(xxVector3::ZERO);
            normals.back().x = ToFloat(strtok_r(nullptr, delim, &lasts));
            normals.back().z = ToFloat(strtok_r(nullptr, delim, &lasts));
            normals.back().y = -ToFloat(strtok_r(nullptr, delim, &lasts));
            break;
        case xxHash("vb"):
            boneIndices.push_back(0);
            boneWeights.push_back(xxVector3::ZERO);
            boneIndices.back() |= ToInt(strtok_r(nullptr, delim, &lasts)) << 0;
            boneWeights.back().x = ToFloat(strtok_r(nullptr, delim, &lasts));
            boneIndices.back() |= ToInt(strtok_r(nullptr, delim, &lasts)) << 8;
            boneWeights.back().y = ToFloat(strtok_r(nullptr, delim, &lasts));
            boneIndices.back() |= ToInt(strtok_r(nullptr, delim, &lasts)) << 16;
            boneWeights.back().z = ToFloat(strtok_r(nullptr, delim, &lasts));
            boneIndices.back() |= ToInt(strtok_r(nullptr, delim, &lasts)) << 24;
            break;
        case xxHash("vc"):
            colors.push_back(xxVector4::ZERO);
            colors.back().r = ToFloat(strtok_r(nullptr, delim, &lasts));
            colors.back().g = ToFloat(strtok_r(nullptr, delim, &lasts));
            colors.back().b = ToFloat(strtok_r(nullptr, delim, &lasts));
            colors.back().a = ToFloat(strtok_r(nullptr, delim, &lasts));
            break;
        case xxHash("mesh"):
            finish();

            xxLog(TAG, "Create Mesh : %s", lasts);
            name = lasts ? lasts : "";
            break;
        case xxHash("material"):
            xxLog(TAG, "Use Material : %s", lasts);
            material = lasts ? &materials[lasts] : nullptr;
            break;
        case xxHash("fm"):
            indices.push_back(ToInt(strtok_r(nullptr, delim, &lasts)));
            indices.push_back(ToInt(strtok_r(nullptr, delim, &lasts)));
            indices.push_back(ToInt(strtok_r(nullptr, delim, &lasts)));
            break;
        case xxHash("joint"):
        {
            char const* jointName = strtok_r(nullptr, delim, &lasts);
            char const* parentName = strtok_r(nullptr, delim, &lasts);
            if (jointName)
            {
                if (jointNodes[jointName] == nullptr)
                {
                    jointNode = xxNode::Create();
                    jointNode->Name = jointName;
                    jointNode->Flags |= xxNode::UPDATE_NEED;

                    jointNodes[jointNode->Name] = jointNode;
                    if (parentName)
                    {
                        xxNodePtr parent = jointNodes[parentName];
                        if (parent)
                        {
                            parent->AttachChild(jointNode);
                        }
                        else
                        {
                            xxLog(TAG, "%s is not found", parentName);
                            root->AttachChild(jointNode);
                        }
                    }
                    else
                    {
                        root->AttachChild(jointNode);
                    }
                }
                else
                {
                    jointNames.push_back(jointName);
                }
            }
            break;
        }
        case xxHash("pm"):
        {
            if (jointNode == nullptr)
                break;

            char const* matrix[12];
            for (int i = 0; i < 12; ++i)
            {
                matrix[i] = strtok_r(nullptr, delim, &lasts);
            }

            xxVector3 translate = { ToFloat(matrix[0]), ToFloat(matrix[1]), ToFloat(matrix[2]) };
            xxMatrix3 rotate;
            rotate[0] = { ToFloat(matrix[3]), ToFloat(matrix[4]), ToFloat(matrix[5]) };
            rotate[1] = { ToFloat(matrix[6]), ToFloat(matrix[7]), ToFloat(matrix[8]) };
            rotate[2] = { ToFloat(matrix[9]), ToFloat(matrix[10]), ToFloat(matrix[11]) };

            static xxMatrix3 swapYZ = { xxVector3::X, -xxVector3::Z, xxVector3::Y };
            translate = translate * rotate.Inverse() * swapYZ;
            translate.x = -translate.x;

            jointNode->SetRotate(rotate);
            jointNode->SetTranslate(translate);

            xxNodePtr const& parent = jointNode->GetParent();
            if (parent)
            {
                jointNode->LocalMatrix = parent->WorldMatrix.Inverse() * jointNode->LocalMatrix;
                jointNode->UpdateMatrix();
            }
            break;
        }
        default:
            break;
        }
    }
    fclose(file);

    finish();

    return root;
}
//==============================================================================
