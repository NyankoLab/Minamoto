//==============================================================================
// Minamoto : ImportEvent Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxNode.h>
#include <xxGraphicPlus/xxTexture.h>
#include <Graphic/Binary.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Tools/NodeTools.h>
#include "Import/ImportFBX.h"
#include "Import/ImportPLY.h"
#include "Import/ImportWavefront.h"
#include "Utility/MeshTools.h"
#include "Utility/TextureTools.h"
#include "ImportEvent.h"

//==============================================================================
//  ImportEvent
//==============================================================================
ImportEvent::ImportEvent(xxNodePtr const& root, std::string name)
{
    static unsigned int accum = 0;
    this->title = std::format("Import {} ({})", xxFile::GetName(name.c_str()), accum++);
    this->root = root;
    this->name = name;
    this->thread = std::thread([this]
    {
        std::shared_ptr<ImportEvent> thiz = this->thiz;
        char const* name = thiz->name.c_str();
        float begin = xxGetCurrentTime();
        xxNodePtr output;
        if (strcasestr(name, ".fbx"))
            output = ImportFBX::Create(name);
        if (strcasestr(name, ".obj"))
        {
            thiz->output = xxNode::Create();
            thiz->output->Name = xxFile::GetName(name);
            ImportWavefront::Create(name, [thiz](xxNodePtr const& node)
            {
                thiz->nodesMutex.lock();
                thiz->nodes.push_back(node);
                thiz->nodesMutex.unlock();
            });
            output = thiz->output;
        }
        if (strcasestr(name, ".ply"))
            output = ImportPLY::Create(name);
        if (strcasestr(name, ".xxb"))
            output = Binary::Load(name);
        xxLog("Hierarchy", "Import : %s (%0.fus)", xxFile::GetName(name).c_str(), (xxGetCurrentTime() - begin) * 1000000);
        thiz->output = output;
        thiz->thread.detach();
    });
}
//------------------------------------------------------------------------------
float ImportEvent::Execute()
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
            for (xxNodePtr const& child : nodes)
            {
                child->Name = Import::CheckDuplicateName(output, child->Name);
                output->AttachChild(child);
            }
            Statistic();
            nodes.clear();
            nodesMutex.unlock();
            root->CreateLinearMatrix();
        }
    }

    bool show = true;
    if (ImGui::Begin(title.c_str(), &show, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking))
    {
        ImGui::InputText("File", name.data(), name.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::InputInt("Node", &nodeCount, 1, 100, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputInt("Mesh", &meshCount, 1, 100, ImGuiInputTextFlags_ReadOnly);
        ImGui::InputInt("Texture", &textureCount, 1, 100, ImGuiInputTextFlags_ReadOnly);
        if (ImGui::Button("QuadTree"))
        {
            NodeTools::ConvertQuadTree(output);
            Statistic();
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Mesh"))
        {
            xxNode::Traversal(output, [](xxNodePtr const& node)
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
        if (ImGui::Button("Mipmap Texture"))
        {
            TextureTools::MipmapTextures(output);
            Statistic();
        }
    }
    ImGui::End();

    if (show)
    {
        return xxGetCurrentTime();
    }
    thiz = nullptr;
    return 0.0f;
}
//------------------------------------------------------------------------------
void ImportEvent::Statistic()
{
    nodeCount = 0;
    meshCount = 0;
    textureCount = 0;

    std::map<size_t, size_t> meshReferenceCounts;
    std::map<size_t, size_t> textureReferenceCounts;
    xxNode::Traversal(output, [&](xxNodePtr const& node)
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
                size_t count = texture.use_count() * node->Material.use_count();
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
