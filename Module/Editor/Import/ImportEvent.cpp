//==============================================================================
// Minamoto : ImportEvent Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Binary.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include <Tools/NodeTools.h>
#include "Import/ImportFilmbox.h"
#include "Import/ImportInterQuake.h"
#include "Import/ImportPolygon.h"
#include "Import/ImportWavefront.h"
#include "Utility/MeshTools.h"
#include "Utility/TextureTools.h"
#include "ImportEvent.h"

//==============================================================================
//  ImportEvent
//==============================================================================
ImportEvent::ImportEvent(xxNodePtr const& root, std::string const& name)
{
    static unsigned int accum = 0;
    this->title = "Import " + xxFile::GetName(name.c_str()) + " (" + std::to_string(accum++) + ")";
    this->root = root;
    this->name = name;
    this->thread = std::thread([this] { this->ThreadedExecute(); });
}
//------------------------------------------------------------------------------
double ImportEvent::Execute()
{
    if (root && output)
    {
        if (output->GetParent() == nullptr)
        {
            if (root->GetParent())
            {
                Import::MergeNode(root, output, root);
            }
            else
            {
                root->AttachChild(output);
            }
            root->CreateLinearMatrix();
        }
        if (nodes.empty() == false && nodesMutex.try_lock())
        {
            mappedNodes[nullptr] = output;
            for (auto [parent, node, right, callback] : nodes)
            {
                if (right == nullptr && node)
                {
                    right = mappedNodes[node];
                }

                if (callback)
                {
                    callback(right);
                    continue;
                }

                if (right == nullptr)
                {
                    continue;
                }
                xxNodePtr left = mappedNodes[parent];
                if (left == nullptr)
                {
                    left = output;
                }
                if (node)
                {
                    mappedNodes[node] = right;
                }
                if (left)
                {
                    right->Name = Import::CheckDuplicateName(left, right->Name);
                    left->AttachChild(right);
                    right->UpdateMatrix();
                }
            }
            Statistic();
            nodes.clear();
            nodesMutex.unlock();
            root->CreateLinearMatrix();
        }
    }

    if (mipmapTextures.empty() == false)
    {
        auto it = mipmapTextures.begin();
        TextureTools::MipmapTexture(*it);
        mipmapTextures.erase(it);
    }

    bool show = true;
    if (ImGui::Begin(title.c_str(), &show, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking))
    {
        ImGui::InputText("File", name.data(), name.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::SliderInt("Node", &nodeCount, 1, 1000, "%d", ImGuiSliderFlags_ReadOnly);
        ImGui::SliderInt("Mesh", &meshCount, 1, 1000, "%d", ImGuiSliderFlags_ReadOnly);
        ImGui::SliderInt("Texture", &textureCount, 1, 1000, "%d", ImGuiSliderFlags_ReadOnly);

        if (ImGui::BeginTable("", 2))
        {
            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Bone");
            ImGui::TableNextColumn();
            if (ImGui::Button("ResetBone"))
            {
                NodeTools::ResetBoneMatrix(output);
                Statistic();
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Mesh");
            ImGui::TableNextColumn();
            if (ImGui::Button("Reset Mesh"))
            {
                Node::Traversal(output, [](xxNodePtr const& node)
                {
                    if (node->Mesh)
                    {
                        xxVector3 origin = node->GetTranslate();
                        node->Mesh = MeshTools::ResetMesh(node->Mesh, origin);
                        node->SetTranslate(origin);
                    }
                    return true;
                });
                Statistic();
            }
            ImGui::SameLine();
            if (ImGui::Button("Unify Mesh"))
            {
                MeshTools::UnifyMesh(output, 1.0f);
                Statistic();
            }
            ImGui::SameLine();
            if (ImGui::Button("Optimize Mesh"))
            {
                Node::Traversal(output, [](xxNodePtr const& node)
                {
                    if (node->Mesh)
                    {
                        MeshTools::OptimizeMesh(node->Mesh);
                    }
                    return true;
                });
                Statistic();
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Node");
            ImGui::TableNextColumn();
            if (ImGui::Button("QuadTree"))
            {
                NodeTools::ConvertQuadTree(output);
                Statistic();
            }

            ImGui::TableNextColumn();
            ImGui::TextUnformatted("Texture");
            ImGui::TableNextColumn();
            if (ImGui::Button("Mipmap Texture") && mipmapTextures.empty())
            {
                Node::Traversal(output, [&](xxNodePtr const& node)
                {
                    if (node->Material)
                    {
                        for (xxTexturePtr const& texture : node->Material->Textures)
                        {
                            mipmapTextures.insert(texture);
                        }
                    }
                    return true;
                });
                Statistic();
            }
            ImGui::EndTable();
        }
    }
    ImGui::End();

    if (show)
    {
        double time;
        xxGetCurrentTime(&time);
        return time;
    }
    thiz = nullptr;
    return 0.0;
}
//------------------------------------------------------------------------------
void ImportEvent::ThreadedExecute()
{
    std::shared_ptr<ImportEvent> thiz = this->thiz;

    float begin = xxGetCurrentTime();

    char const* name = thiz->name.c_str();
    xxNodePtr output;
    if (strcasestr(name, ".fbx"))
    {
#if 1
        thiz->output = xxNode::Create();
        thiz->output->Name = xxFile::GetName(name);
        output = ImportFilmbox::Create(name, [thiz](void* parent, void* node, xxNodePtr&& target, std::function<void(xxNodePtr const&)> callback)
        {
            thiz->nodesMutex.lock();
            thiz->nodes.emplace_back(parent, node, std::move(target), std::move(callback));
            thiz->nodesMutex.unlock();
        });
        output = thiz->output;
#else
        output = ImportFilmbox::Create(name);
#endif
    }
    if (strcasestr(name, ".iqe"))
    {
#if 1
        output = ImportInterQuake::Create(name);
#else
        thiz->output = xxNode::Create();
        thiz->output->Name = xxFile::GetName(name);
        ImportInterQuake::Create(name, [thiz](xxNodePtr&& target)
        {
            thiz->nodesMutex.lock();
            thiz->nodes.emplace_back(nullptr, nullptr, std::move(target), nullptr);
            thiz->nodesMutex.unlock();
        });
        output = thiz->output;
#endif
    }
    if (strcasestr(name, ".obj"))
    {
        thiz->output = xxNode::Create();
        thiz->output->Name = xxFile::GetName(name);
        ImportWavefront::Create(name, [thiz](xxNodePtr&& target)
        {
            thiz->nodesMutex.lock();
            thiz->nodes.emplace_back(nullptr, nullptr, std::move(target), nullptr);
            thiz->nodesMutex.unlock();
        });
        output = thiz->output;
    }
    if (strcasestr(name, ".ply"))
        output = ImportPolygon::Create(name);
    if (strcasestr(name, ".xxb"))
        output = Binary::Load(name);
    thiz->output = output;

    xxLog("Hierarchy", "Import : %s (%0.fus)", xxFile::GetName(name).c_str(), (xxGetCurrentTime() - begin) * 1000000);

    thiz->thread.detach();
}
//------------------------------------------------------------------------------
void ImportEvent::Statistic()
{
    nodeCount = 0;
    meshCount = 0;
    textureCount = 0;

    std::map<size_t, size_t> meshReferenceCounts;
    std::map<size_t, size_t> textureReferenceCounts;
    Node::Traversal(output, [&](xxNodePtr const& node)
    {
        if (node->Mesh)
        {
            size_t count = node->Mesh.use_count();
            meshReferenceCounts[count]++;
        }
        if (node->Material)
        {
            for (xxTexturePtr const& texture : node->Material->Textures)
            {
                if (texture == nullptr)
                    continue;
                size_t count = 0;
                if (mipmapTextures.find(texture) == mipmapTextures.end())
                {
                    count = texture.use_count() * node->Material.use_count();
                }
                else
                {
                    count = (texture.use_count() - 1) * node->Material.use_count();
                }
                textureReferenceCounts[count]++;
            }
        }
        return true;
    });
    nodeCount = (int)output->GetChildCount();
    for (auto [size, count] : meshReferenceCounts)
    {
        meshCount += count / size;
    }
    for (auto [size, count] : textureReferenceCounts)
    {
        textureCount += count / size;
    }
}
//------------------------------------------------------------------------------
std::shared_ptr<ImportEvent> ImportEvent::Create(xxNodePtr const& root, std::string name)
{
    auto event = std::make_shared<ImportEvent>(root, name);
    event->thiz = event;
    return event;
}
//==============================================================================
