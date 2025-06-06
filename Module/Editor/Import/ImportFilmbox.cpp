//==============================================================================
// Minamoto : ImportFilmbox Source
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
#include <Runtime/Modifier/ConstantQuaternionModifier.h>
#include <Runtime/Modifier/ConstantScaleModifier.h>
#include <Runtime/Modifier/ConstantTranslateModifier.h>
#include <Runtime/Modifier/QuaternionModifier.h>
#include <Runtime/Modifier/ScaleModifier.h>
#include <Runtime/Modifier/TranslateModifier.h>
#include <Runtime/Modifier/BakedQuaternionModifier.h>
#include <Runtime/Modifier/Quaternion16Modifier.h>
#include <Runtime/Modifier/BakedQuaternion16Modifier.h>
#include "Utility/MeshTools.h"
#include "ImportFilmbox.h"

#include "ufbx/ufbx.h"

#define TAG "ImportFilmbox"

//==============================================================================
static std::string str(const ufbx_string& string)
{
    return std::string(string.data, string.length);
}
//------------------------------------------------------------------------------
static xxVector2 vec2(const ufbx_vec2& values)
{
    return { values.x, values.y };
}
//------------------------------------------------------------------------------
static xxVector3 vec3(const ufbx_vec3& values)
{
    return { values.x, values.y, values.z };
}
//------------------------------------------------------------------------------
static xxVector4 vec4(const ufbx_vec4& values)
{
    return { values.x, values.y, values.z, values.w };
}
//------------------------------------------------------------------------------
static xxVector4 quat(const ufbx_quat& values)
{
    return { values.x, values.y, values.z, values.w };
}
//------------------------------------------------------------------------------
static xxMatrix4x4 mat4(const ufbx_matrix& values)
{
    xxMatrix4x4 output = xxMatrix4x4::IDENTITY;
    output[0].xyz = vec3(values.cols[0]);
    output[1].xyz = vec3(values.cols[1]);
    output[2].xyz = vec3(values.cols[2]);
    output[3].xyz = vec3(values.cols[3]);
    return output;
}
//------------------------------------------------------------------------------
static xxMatrix4x4 mat4(const ufbx_transform& values)
{
    return mat4(ufbx_transform_to_matrix(&values));
}
//------------------------------------------------------------------------------
static xxTexturePtr CreateTexture(ufbx_texture* texture)
{
    if (texture == nullptr || texture->has_file == false)
        return xxTexturePtr();
    xxTexturePtr output = Import::CreateTexture(texture->filename.data);
    if (output)
    {
        output->ClampU = (texture->wrap_u == UFBX_WRAP_CLAMP);
        output->ClampV = (texture->wrap_v == UFBX_WRAP_CLAMP);
        output->ClampW = (texture->wrap_u == UFBX_WRAP_CLAMP) && (texture->wrap_v == UFBX_WRAP_CLAMP);
    }
    return output;
}
//------------------------------------------------------------------------------
static void CreateAnimation(ufbx_scene* scene, xxNodePtr const& root, Import::ImportCallback callback)
{
    ufbx_bake_opts opts = {};
    ufbx_error error;
    opts.key_reduction_enabled = true;
    opts.key_reduction_rotation = true;
    opts.key_reduction_passes = 16;
    ufbx_baked_anim* baked = ufbx_bake_anim(scene, nullptr, nullptr, &error);
    ufbx_baked_anim* reduction = ufbx_bake_anim(scene, nullptr, &opts, &error);
    if (reduction == nullptr)
    {
        xxLog(TAG, "CreateAnimation : %s", error.info);
        return;
    }

    // Create
    for (size_t i = 0; i < baked->nodes.count; ++i)
    {
        ufbx_baked_node& baked_node = baked->nodes.data[i];
        ufbx_baked_node& reduction_node = reduction->nodes.data[i];
        ufbx_element* element = scene->elements[reduction_node.element_id];
        if (element->type != UFBX_ELEMENT_NODE)
        {
            xxLog(TAG, "CreateAnimation : %s is not node", element->name.data);
            continue;
        }
        ufbx_node* target_node = nullptr;
        xxNodePtr target;
        if (callback)
        {
            target_node = ufbx_find_node_len(element->scene, element->name.data, element->name.length);
        }
        else
        {
            target = Import::GetNodeByName(root, element->name.data);
        }
        if (target_node == nullptr && target == nullptr)
        {
            xxLog(TAG, "CreateAnimation : %s is not found", element->name.data);
            continue;
        }

        // Rotation
        size_t rotation = 0;
        if (reduction_node.rotation_keys.count)
        {
            xxModifierPtr modifier;
            if (reduction_node.constant_rotation)
            {
                modifier = ConstantQuaternionModifier::Create(quat(reduction_node.rotation_keys[0].value));
                rotation = 1;
            }
            else if (Modifier::CalculateSize(Modifier::QUATERNION16, reduction_node.rotation_keys.count) < Modifier::CalculateSize(Modifier::BAKED_QUATERNION16, baked_node.rotation_keys.count))
            {
                modifier = Quaternion16Modifier::Create(reduction_node.rotation_keys.count, [&](size_t index, float& time, xxVector4& quaternion)
                {
                    time = float(reduction_node.rotation_keys.data[index].time - reduction->key_time_min);
                    quaternion = quat(reduction_node.rotation_keys.data[index].value);
                });
                rotation = reduction_node.rotation_keys.count;
            }
            else
            {
                modifier = BakedQuaternion16Modifier::Create(baked_node.rotation_keys.count, float(baked->playback_duration), [&](size_t index, xxVector4& quaternion)
                {
                    quaternion = quat(baked_node.rotation_keys.data[index].value);
                });
                rotation = baked_node.rotation_keys.count;
            }
            if (callback)
            {
                callback(nullptr, target_node, nullptr, [modifier](xxNodePtr const& target)
                {
                    target->Modifiers.emplace_back(modifier);
                });
            }
            else
            {
                target->Modifiers.emplace_back(modifier);
            }
        }

        // Translate
        size_t translate = 0;
        if (reduction_node.translation_keys.count)
        {
            xxModifierPtr modifier;
            if (reduction_node.constant_translation)
            {
                modifier = ConstantTranslateModifier::Create(vec3(reduction_node.translation_keys[0].value));
                translate = 1;
            }
            else
            {
                modifier = TranslateModifier::Create(reduction_node.translation_keys.count, [&](size_t index, float& time, xxVector3& translate)
                {
                    time = float(reduction_node.translation_keys.data[index].time - reduction->key_time_min);
                    translate = vec3(reduction_node.translation_keys.data[index].value);
                });
                translate = reduction_node.translation_keys.count;
            }
            if (callback)
            {
                callback(nullptr, target_node, nullptr, [modifier](xxNodePtr const& target)
                {
                    target->Modifiers.emplace_back(modifier);
                });
            }
            else
            {
                target->Modifiers.emplace_back(modifier);
            }
        }

        // Scale
        size_t scale = 0;
        if (reduction_node.scale_keys.count)
        {
            xxModifierPtr modifier;
            if (reduction_node.constant_scale)
            {
                modifier = ConstantScaleModifier::Create(reduction_node.scale_keys[0].value.x);
                scale = 1;
            }
            else
            {
                modifier = ScaleModifier::Create(reduction_node.scale_keys.count, [&](size_t index, float& time, float& scale)
                {
                    time = float(reduction_node.scale_keys.data[index].time - reduction->key_time_min);
                    scale = float(reduction_node.scale_keys.data[index].value.x);
                });
                scale = reduction_node.scale_keys.count;
            }
            if (callback)
            {
                callback(nullptr, target_node, nullptr, [modifier](xxNodePtr const& target)
                {
                    target->Modifiers.emplace_back(modifier);
                });
            }
            else
            {
                target->Modifiers.emplace_back(modifier);
            }
        }

        xxLog(TAG, "CreateAnimation : Rotation %zd, Translate %zd, Scale %zd - %s", rotation, translate, scale, element->name.data);
    }

    ufbx_free_baked_anim(baked);
    ufbx_free_baked_anim(reduction);
}
//------------------------------------------------------------------------------
static xxMaterialPtr CreateMaterial(ufbx_material* material)
{
    if (material == nullptr)
        return xxMaterialPtr();
    xxMaterialPtr output = xxMaterial::Create();
    output->Name = str(material->name);
    output->AmbientColor = vec3(material->fbx.ambient_color.value_vec3);
    output->DiffuseColor = vec3(material->fbx.diffuse_color.value_vec3);
    output->EmissiveColor = vec3(material->fbx.emission_color.value_vec3);
    output->SpecularColor = vec3(material->fbx.specular_color.value_vec3);
    output->SpecularHighlight = material->fbx.specular_exponent.value_real;
    output->Opacity = material->fbx.transparency_factor.value_real;
    switch (material->shader_type)
    {
    default:
    case UFBX_SHADER_FBX_LAMBERT:
        output->Lighting = true;
        break;
    case UFBX_SHADER_FBX_PHONG:
        output->Lighting = true;
        output->Specular = true;
        break;
    }
    output->DepthTest = "LessEqual";
    output->DepthWrite = true;
    output->Cull = true;
    output->Scissor = false;
    for (size_t i = 0; i < material->textures.count; ++i)
    {
        output->SetTexture(i, CreateTexture(material->textures.data[i].texture));
    }
    return output;
};
//------------------------------------------------------------------------------
static xxMeshPtr CreateMesh(ufbx_mesh* mesh, xxNodePtr const& node, xxNodePtr const& root)
{
    if (mesh == nullptr)
        return xxMeshPtr();
    ufbx_skin_deformer* skin_deformer = mesh->skin_deformers.count ? mesh->skin_deformers.data[0] : nullptr;
    int normalCount = 0;
    int colorCount = 0;
    int textureCount = 0;
    if (mesh->vertex_normal.exists)
    {
        normalCount = 1;
        if (mesh->vertex_tangent.exists)
        {
            normalCount = 2;
            if (mesh->vertex_bitangent.exists)
            {
                normalCount = 3;
            }
        }
    }
    colorCount = std::min((int)mesh->color_sets.count, 8);
    textureCount = std::min((int)mesh->uv_sets.count, 8);
    xxMeshPtr output = xxMesh::Create(skin_deformer != nullptr, normalCount, colorCount, textureCount);
    output->Name = str(mesh->name);

    bool indices_equal = true;
    if (normalCount >= 1)
        indices_equal &= (mesh->vertex_normal.values.count == mesh->vertex_position.values.count);
    if (normalCount >= 2)
        indices_equal &= (mesh->vertex_tangent.values.count == mesh->vertex_position.values.count);
    if (normalCount >= 3)
        indices_equal &= (mesh->vertex_bitangent.values.count == mesh->vertex_position.values.count);
    for (int j = 0; j < colorCount; ++j)
        indices_equal &= (mesh->color_sets[j].vertex_color.values.count == mesh->vertex_position.values.count);
    for (int j = 0; j < textureCount; ++j)
        indices_equal &= (mesh->uv_sets[j].vertex_uv.values.count == mesh->vertex_position.values.count);

    if (indices_equal)
    {
        output->SetVertexCount(static_cast<int>(mesh->vertex_position.values.count));
        output->SetIndexCount(static_cast<int>(mesh->num_indices));
    }
    else
    {
        output->SetVertexCount(static_cast<int>(mesh->num_indices));
    }

    xxStrideIterator<xxVector3> positions = output->GetPosition();
    xxStrideIterator<xxVector3> boneWeight = output->GetBoneWeight();
    xxStrideIterator<uint32_t> boneIndices = output->GetBoneIndices();
    xxStrideIterator<uint32_t> normals = output->GetNormal(0);
    xxStrideIterator<uint32_t> tangents = output->GetNormal(1);
    xxStrideIterator<uint32_t> bitangents = output->GetNormal(2);
    xxStrideIterator<uint32_t> colors[8] =
    {
        output->GetColor(0), output->GetColor(1), output->GetColor(2), output->GetColor(3),
        output->GetColor(4), output->GetColor(5), output->GetColor(6), output->GetColor(7),
    };
    xxStrideIterator<xxVector2> textures[8] =
    {
        output->GetTexture(0), output->GetTexture(1), output->GetTexture(2), output->GetTexture(3),
        output->GetTexture(4), output->GetTexture(5), output->GetTexture(6), output->GetTexture(7),
    };

    if (indices_equal)
    {
        for (size_t i = 0; i < mesh->vertex_position.values.count; ++i)
        {
            (*positions++) = vec3(mesh->vertex_position.values[i]);
            if (skin_deformer)
            {
                uint32_t weightBegin = skin_deformer->vertices.data[i].weight_begin;
                uint32_t numWeights = skin_deformer->vertices.data[i].num_weights;
                xxVector4 weight = xxVector4::ZERO;
                uint32_t indices = 0;
                for (uint32_t i = 0; i < numWeights && i < 4; ++i)
                {
                    weight[i] = skin_deformer->weights.data[weightBegin + i].weight;
                    indices |= skin_deformer->weights.data[weightBegin + i].cluster_index << (i * 8);
                }
                (*boneWeight++) = weight.xyz;
                (*boneIndices++) = indices;
            }
            if (normalCount >= 1)
                (*normals++) = Mesh::NormalEncode(vec3(mesh->vertex_normal.values[i]));
            if (normalCount >= 2)
                (*tangents++) = Mesh::NormalEncode(vec3(mesh->vertex_tangent.values[i]));
            if (normalCount >= 3)
                (*bitangents++) = Mesh::NormalEncode(vec3(mesh->vertex_bitangent.values[i]));
            for (int j = 0; j < colorCount; ++j)
                (*colors[j]++) = vec4(mesh->color_sets[j].vertex_color.values[i]).ToInteger();
            for (int j = 0; j < textureCount; ++j)
                (*textures[j]++) = vec2(mesh->uv_sets[j].vertex_uv.values[i]);
        }

        if (mesh->vertex_position.values.count < 65536)
        {
            uint16_t* indices = reinterpret_cast<uint16_t*>(output->Index);
            for (size_t i = 0; i < mesh->num_indices; ++i)
            {
                (*indices++) = mesh->vertex_position.indices.data[i];
            }
        }
        else
        {
            uint32_t* indices = reinterpret_cast<uint32_t*>(output->Index);
            for (size_t i = 0; i < mesh->num_indices; ++i)
            {
                (*indices++) = mesh->vertex_position.indices.data[i];
            }
        }
    }
    else
    {
        for (size_t i = 0; i < mesh->num_indices; ++i)
        {
            (*positions++) = vec3(ufbx_get_vertex_vec3(&mesh->vertex_position, i));
            if (skin_deformer)
            {
                uint32_t index = mesh->vertex_position.indices.data[i];
                uint32_t weightBegin = skin_deformer->vertices.data[index].weight_begin;
                uint32_t numWeights = skin_deformer->vertices.data[index].num_weights;
                xxVector4 weight = xxVector4::ZERO;
                uint32_t indices = 0;
                for (uint32_t i = 0; i < numWeights && i < 4; ++i)
                {
                    weight[i] = skin_deformer->weights.data[weightBegin + i].weight;
                    indices |= skin_deformer->weights.data[weightBegin + i].cluster_index << (i * 8);
                }
                (*boneWeight++) = weight.xyz;
                (*boneIndices++) = indices;
            }
            if (normalCount >= 1)
                (*normals++) = Mesh::NormalEncode(vec3(ufbx_get_vertex_vec3(&mesh->vertex_normal, i)));
            if (normalCount >= 2)
                (*tangents++) = Mesh::NormalEncode(vec3(ufbx_get_vertex_vec3(&mesh->vertex_tangent, i)));
            if (normalCount >= 3)
                (*bitangents++) = Mesh::NormalEncode(vec3(ufbx_get_vertex_vec3(&mesh->vertex_bitangent, i)));
            for (int j = 0; j < colorCount; ++j)
                (*colors[j]++) = vec4(ufbx_get_vertex_vec4(&mesh->color_sets[j].vertex_color, i)).ToInteger();
            for (int j = 0; j < textureCount; ++j)
                (*textures[j]++) = vec2(ufbx_get_vertex_vec2(&mesh->uv_sets[j].vertex_uv, i));
        }
    }
    output->CalculateBound();

    return output;
}
//------------------------------------------------------------------------------
static void CreateSkinning(ufbx_mesh* mesh, xxNodePtr const& node, xxNodePtr const& root, Import::ImportCallback callback)
{
    ufbx_skin_deformer* skin_deformer = mesh->skin_deformers.count ? mesh->skin_deformers.data[0] : nullptr;

    if (skin_deformer)
    {
        for (size_t i = 0; i < skin_deformer->clusters.count; ++i)
        {
            ufbx_skin_cluster* cluster = skin_deformer->clusters.data[i];

            // Bound
            xxVector4 bound = xxVector4::ZERO;
            for (size_t j = 0; j < cluster->vertices.count; ++j)
            {
                uint32_t index = cluster->vertices.data[j];
                xxVector3 vertex = vec3(mesh->vertices[index]);
                bound.BoundMerge(vertex);
            }
            bound = bound.BoundTransform(mat4(cluster->geometry_to_bone), 1.0f);

            // SkinMatrix
            xxMatrix4x4 skinMatrix = mat4(cluster->geometry_to_bone);

            // Find the bone
            ufbx_node* from = cluster->bone_node;

            // Callback
            if (callback)
            {
                std::string name = from ? from->name.data : "(nullptr)";
                callback(nullptr, from, nullptr, [node, bound, skinMatrix, name](xxNodePtr const& to)
                {
                    if (to == nullptr)
                    {
                        xxLog(TAG, "Bone %s is not found", name.c_str());
                        return;
                    }
                    xxNode::BoneData bone;
                    bone.bone = to;
                    bone.bound = bound;
                    bone.classSkinMatrix = skinMatrix;
                    bone.classBoneMatrix = {};
                    node->Bones.push_back(bone);
                    for (auto& data : node->Bones)
                    {
                        data.ResetPointer();
                    }
                });
                continue;
            }

            // Find the bone node
            xxNodePtr to;
            if (from)
            {
                char const* name = from->name.data;
                Node::Traversal(root, [&](xxNodePtr const& node)
                {
                    if (node->Name == name)
                        to = node;
                    return to == nullptr;
                });
            }
            if (to == nullptr)
            {
                to = root;
                xxLog(TAG, "Bone %s is not found", from ? from->name.data : "(nullptr)");
            }

            if (isnan(bound.radius))
            {
                xxLog(TAG, "NaN");
            }

            // Add to list
            xxNode::BoneData bone;
            bone.bone = to;
            bone.bound = bound;
            bone.classSkinMatrix = skinMatrix;
            bone.classBoneMatrix = {};
            node->Bones.push_back(bone);
        }
        for (auto& data : node->Bones)
        {
            data.ResetPointer();
        }
    }
}
//------------------------------------------------------------------------------
static xxNodePtr CreateNode(ufbx_node* node, xxNodePtr root, Import::ImportCallback callback)
{
    if (node == nullptr)
        return xxNodePtr();
    xxNodePtr output = xxNode::Create();
    if (root == nullptr)
        root = output;
    output->Name = str(node->name);
    output->LocalMatrix = mat4(node->local_transform);
    if (callback)
    {
        if (node->parent)
        {
            callback(node->parent, node, xxNodePtr(output), nullptr);
        }
        for (size_t i = 0; i < node->children.count; ++i)
        {
            CreateNode(node->children.data[i], root, callback);
        }
    }
    else
    {
        for (size_t i = 0; i < node->children.count; ++i)
        {
            xxNodePtr child = CreateNode(node->children.data[i], root, callback);
            if (child)
            {
                output->AttachChild(child);
                child->UpdateMatrix();
            }
        }
    }
    if (node->mesh)
    {
        xxNodePtr geometryNode = output;
        if (node->has_geometry_transform)
        {
            if (node->children.count == 0)
            {
                ufbx_matrix local_transform = ufbx_transform_to_matrix(&node->local_transform);
                ufbx_matrix geometry_transform = ufbx_transform_to_matrix(&node->geometry_transform);
                output->LocalMatrix = mat4(ufbx_matrix_mul(&local_transform, &geometry_transform));
            }
            else
            {
                geometryNode = xxNode::Create();
                geometryNode->Name = str(node->mesh->name);
                geometryNode->LocalMatrix = mat4(node->geometry_transform);
            }
        }
        if (node->mesh->materials.count)
        {
            ufbx_material* material = node->mesh->materials.data[0];
            geometryNode->Material = CreateMaterial(material);
        }
        geometryNode->Mesh = CreateMesh(node->mesh, geometryNode, root);
        if (Import::EnableOptimizeMesh)
        {
            geometryNode->Mesh = MeshTools::IndexingMesh(geometryNode->Mesh);
        }

        CreateSkinning(node->mesh, geometryNode, root, callback);

        if (geometryNode != output)
        {
            if (callback)
            {
                callback(node, node->mesh, std::move(geometryNode), nullptr);
            }
            else
            {
                output->AttachChild(geometryNode);
                geometryNode->UpdateMatrix();
            }
        }
    }
    return output;
}
//------------------------------------------------------------------------------
xxNodePtr ImportFilmbox::Create(char const* fbx, ImportCallback callback)
{
    ufbx_load_opts opts = {};
    ufbx_error error;
    ufbx_scene* scene = ufbx_load_file(fbx, &opts, &error);
    if (scene == nullptr)
    {
        xxLog(TAG, "Failed to load: %s", error.description.data);
        return nullptr;
    }

    xxNodePtr root = CreateNode(scene->root_node, nullptr, callback);
    if (root)
    {
        if (root->Name.empty())
        {
            root->Name = xxFile::GetName(fbx);
        }

        if (EnableAxisUpYToZ)
        {
            static xxMatrix4 const YtoZ =
            {
                1,  0, 0, 0,
                0,  0, 1, 0,
                0, -1, 0, 0,
                0,  0, 0, 1,
            };
            root->LocalMatrix = root->LocalMatrix * YtoZ;
        }

        CreateAnimation(scene, root, callback);
    }

    ufbx_free_scene(scene);

    return root;
}
//==============================================================================
