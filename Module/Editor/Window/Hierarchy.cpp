//==============================================================================
// Minamoto : Hierarchy Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <Runtime/Graphic/Binary.h>
#include <Runtime/Graphic/Camera.h>
#include <Runtime/Graphic/Node.h>
#include <Runtime/MiniGUI/Window.h>
#include <Runtime/Tools/NodeTools.h>
#include "Export/ExportEvent.h"
#include "Import/Import.h"
#include "Import/ImportEvent.h"
#include "Import/ImportFilmbox.h"
#include "Import/ImportPolygon.h"
#include "Import/ImportWavefront.h"
#include "Utility/ParticleTools.h"
#include "Utility/Tools.h"
#include "Hierarchy.h"
#include "Log.h"
#include "Scene.h"
#include "Inspector.h"

#define TAG "Hierarchy"

float Hierarchy::windowPosY = 0.0f;
float Hierarchy::windowWidth = 256.0f;
xxNodePtr Hierarchy::selectedLeft;
xxNodePtr Hierarchy::selectedRight;
xxNodePtr Hierarchy::importNode;
xxNodePtr Hierarchy::exportNode;
char Hierarchy::importName[1024];
char Hierarchy::exportName[1024];
//==============================================================================
static void AddCamera(xxNodePtr const& root)
{
    auto node = xxNode::Create();
    node->Camera = xxCamera::Create();
    node->Camera->Up = xxVector3::Z;
    node->Camera->Right = -xxVector3::X;
    node->Camera->Direction = xxVector3::Y;
    node->Camera->Location = root->WorldBound.xyz - xxVector3::Y * root->WorldBound.z * 2.0f;
    node->Camera->SetFOV(16.0f / 9.0f, 60.0f, 10000.0f);
    node->Camera->LightDirection.y = -1.0f;
    root->AttachChild(node);
}
//------------------------------------------------------------------------------
static void AddNode(xxNodePtr const& root)
{
    auto node = xxNode::Create();
    root->AttachChild(node);
}
//------------------------------------------------------------------------------
static void AddParticle(xxNodePtr const& root)
{
    auto node = ParticleTools::CreateParticle(0);
    root->AttachChild(node);
}
//------------------------------------------------------------------------------
#if HAVE_MINIGUI
static void AddWindow(xxNodePtr const& root)
{
    auto window = MiniGUI::Window::Create();
    root->AttachChild(window);
}
#endif
//------------------------------------------------------------------------------
void Hierarchy::Initialize()
{
    selectedLeft = nullptr;
    selectedRight = nullptr;
    importNode = nullptr;
    exportNode = nullptr;
}
//------------------------------------------------------------------------------
void Hierarchy::Shutdown()
{
    selectedLeft = nullptr;
    selectedRight = nullptr;
    importNode = nullptr;
    exportNode = nullptr;
}
//------------------------------------------------------------------------------
static xxNodePtr ImportFile(xxNodePtr const& node, char const* name)
{
    if (node)
    {
        Event::Push(ImportEvent::Create(node, name));
        return nullptr;
    }

    float begin = xxGetCurrentTime();
    xxNodePtr output;
    if (strcasestr(name, ".fbx"))
        output = ImportFilmbox::Create(name);
    if (strcasestr(name, ".obj"))
        output = ImportWavefront::Create(name);
    if (strcasestr(name, ".ply"))
        output = ImportPolygon::Create(name);
    if (strcasestr(name, ".xxb"))
        output = Binary::Load(name);
    if (output)
    {
        xxLog("Hierarchy", "Import : %s (%0.fus)", xxFile::GetName(name).c_str(), (xxGetCurrentTime() - begin) * 1000000);
    }
    return output;
}
//------------------------------------------------------------------------------
void Hierarchy::Select(xxNodePtr const& node)
{
    selectedLeft = node;
}
//------------------------------------------------------------------------------
void Hierarchy::Import(const UpdateData& updateData)
{
    if (importNode == nullptr)
        return;

    bool show = true;
    if (ImGui::Begin("Import", &show, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SetNextItemWidth(384 * updateData.scale);
        ImGui::InputText("Path", importName, 1024);
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            if (importName[0] == 0 && exportName[0])
                strcpy(importName, exportName);
        }
        ImGui::Checkbox("Axis Up Y to Z", &Import::EnableAxisUpYToZ);
        ImGui::Checkbox("Merge Node", &Import::EnableMergeNode);
        ImGui::Checkbox("Merge Texture", &Import::EnableMergeTexture);
        ImGui::Checkbox("Optimize Mesh", &Import::EnableOptimizeMesh);
        ImGui::Checkbox("Texture Flip V", &Import::EnableTextureFlipV);
        if (ImGui::Button("Import"))
        {
            xxNodePtr node = ImportFile(nullptr, importName);
            if (node)
            {
                if (Import::EnableMergeNode)
                {
                    Import::MergeNode(importNode, node, importNode);
                }
                else
                {
                    importNode->AttachChild(node);
                }
                if (Import::EnableMergeTexture)
                {
                    Import::MergeTexture(node);
                }

                xxNodePtr const& root = NodeTools::GetRoot(importNode);
                root->CreateLinearMatrix();

                importNode = nullptr;
                show = false;
            }
        }
    }
    ImGui::End();

    if (show == false)
    {
        importNode = nullptr;
    }
}
//------------------------------------------------------------------------------
void Hierarchy::Export(const UpdateData& updateData)
{
    if (exportNode == nullptr)
        return;

    bool show = true;
    if (ImGui::Begin("Export", &show, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::SetNextItemWidth(384 * updateData.scale);
        ImGui::InputText("Path", exportName, 1024);
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
            if (exportName[0] == 0 && importName[0])
                strcpy(exportName, importName);
        }
        if (ImGui::Button("Export"))
        {
            Node::Traversal(exportNode, [&](xxNodePtr const& node)
            {
                node->Flags &= ~NodeTools::TEST_CHECK_FLAG;
                return true;
            });
            float begin = xxGetCurrentTime();
            if (Binary::Save(exportName, exportNode))
            {
                xxLog("Hierarchy", "Export : %s (%0.fus)", xxFile::GetName(exportName).c_str(), (xxGetCurrentTime() - begin) * 1000000);
                exportNode = nullptr;
                show = false;
            }
        }
    }
    ImGui::End();

    if (show == false)
    {
        exportNode = nullptr;
    }
}
//------------------------------------------------------------------------------
bool Hierarchy::Update(const UpdateData& updateData, bool& show, xxNodePtr const& root)
{
    if (show == false)
        return false;

    bool update = false;
    xxNodePtr hovered;
    if (ImGui::Begin(ICON_FA_LIST "Hierarchy", &show))
    {
        windowPosY = ImGui::GetCursorPosY();
        windowWidth = ImGui::GetWindowWidth();

        auto dragFile = [](xxNodePtr const& node)
        {
            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DRAGFILE");
                if (payload)
                {
                    std::string name((char*)payload->Data, payload->DataSize);
                    xxNodePtr object = ImportFile(node, name.c_str());
                    if (object)
                    {
                        if (node->GetParent())
                        {
                            Import::MergeNode(node, object, node);
                        }
                        else
                        {
                            node->AttachChild(object);
                        }

                        xxNodePtr const& root = NodeTools::GetRoot(node);
                        root->CreateLinearMatrix();
                    }
                }
                ImGui::EndDragDropTarget();
            }
        };

        char name[256];
        bool clickedLeft = false;
        bool clickedRight = false;
        size_t TEST_OPEN_FLAG = xxNode::RESERVED0;
        auto traversal = [&](auto&& traversal, xxNodePtr const& node) -> void
        {
            if (node == nullptr)
                return;

            // TODO -
            if (ImGui::IsMouseDragging(ImGuiMouseButton_Left))
            {
                ImVec2 min = ImGui::GetCursorScreenPos();
                ImVec2 max;
                max.x = min.x + ImGui::GetContentRegionAvail().x;
                max.y = min.y + ImGui::GetFrameHeight();
                if (ImGui::IsMouseHoveringRect(min, max))
                {
                    ImGui::GetWindowDrawList()->AddRect(min, max, ImGui::GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
                }
            }

            bool opened = (node->Flags & TEST_OPEN_FLAG) != 0;
            char const* type;
            if (node->Camera)
                type = ICON_FA_CAMERA;
            else if (node->Mesh)
                type = ICON_FA_CUBE;
            else
                type = opened ? ICON_FA_CIRCLE_O : ICON_FA_CIRCLE;
            if (node->Name.empty())
                snprintf(name, sizeof(name), "%s%p", type, node.get());
            else
                snprintf(name, sizeof(name), "%s%s", type, node->Name.c_str());
            ImGui::Selectable(name, selectedLeft == node);

            // Hovered
            if (ImGui::IsItemHovered())
            {
                hovered = node;

                // Left button
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
                {
                    selectedLeft = node;
                    Inspector::Select(node);
                    Scene::Select(node);
                    clickedLeft = true;
                }
                if (ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
                {
                    node->Flags &= ~TEST_OPEN_FLAG;
                    if (opened == false && node->GetChildCount() != 0)
                        node->Flags |= TEST_OPEN_FLAG;
                }

                // Right button
                if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
                {
                    selectedRight = node;
                    clickedRight = true;
                }

                // Drag
                dragFile(node);
            }

            // Traversal
            if (opened)
            {
                ImGui::Indent();
                for (xxNodePtr const& child : *node)
                    traversal(traversal, child);
                ImGui::Unindent();
            }
        };
        if (root)
        {
            for (xxNodePtr const& child : *root)
                traversal(traversal, child);

            // Drag
            ImVec2 avail = ImGui::GetContentRegionAvail();
            if (avail.x && avail.y)
            {
                ImGui::InvisibleButton("", avail);
                dragFile(root);
            }
        }

        // Left
        if (clickedLeft == false && selectedLeft && ImGui::IsWindowHovered())
        {
            xxNodePtr const& parent = selectedLeft->GetParent();
            if (parent)
            {
                size_t index = parent->GetChildCount();
                for (size_t i = 0; i < parent->GetChildCount(); ++i)
                {
                    if (parent->GetChild(i) == selectedLeft)
                    {
                        index = i;
                        break;
                    }
                }

                if (ImGui::IsKeyPressed(ImGuiKey_UpArrow) || ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                {
                    if (ImGui::IsKeyPressed(ImGuiKey_UpArrow))
                        index--;
                    if (ImGui::IsKeyPressed(ImGuiKey_DownArrow))
                        index++;
                    if (index < parent->GetChildCount())
                    {
                        update = true;
                        selectedLeft = parent->GetChild(index);
                        Inspector::Select(selectedLeft);
                        Scene::Select(selectedLeft);
                    }
                }
            }

            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                selectedLeft = nullptr;
                Inspector::Select(nullptr);
                Scene::Select(nullptr);
            }
        }

        // Right
        if (clickedRight == false && ImGui::IsWindowHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Right))
        {
            selectedRight = root;
            clickedRight = true;
        }

        // Popup
        if (clickedRight)
        {
            ImGui::OpenPopup("node");
        }
        if (selectedRight && ImGui::BeginPopup("node"))
        {
            if (selectedRight->Name.empty())
                ImGui::Text("%p", selectedRight.get());
            else
                ImGui::Text("%s", selectedRight->Name.c_str());
            ImGui::Separator();
#if HAVE_MINIGUI
            if (selectedRight && (selectedRight == root || MiniGUI::Window::Cast(selectedRight) != nullptr) && ImGui::Button("Add Camera"))
#else
            if (selectedRight && ImGui::Button("Add Camera"))
#endif
            {
                update = true;
                AddCamera(selectedRight);
                selectedRight->Flags |= TEST_OPEN_FLAG;
                selectedRight = nullptr;
            }
#if HAVE_MINIGUI
            if (selectedRight && (selectedRight == root || MiniGUI::Window::Cast(selectedRight) == nullptr) && ImGui::Button("Add Node"))
#else
            if (selectedRight && ImGui::Button("Add Node"))
#endif
            {
                update = true;
                AddNode(selectedRight);
                selectedRight->Flags |= TEST_OPEN_FLAG;
                selectedRight = nullptr;
            }
#if HAVE_MINIGUI
            if (selectedRight && (selectedRight == root || MiniGUI::Window::Cast(selectedRight) != nullptr) && ImGui::Button("Add Particle"))
#else
            if (selectedRight && ImGui::Button("Add Particle"))
#endif
            {
                update = true;
                AddParticle(selectedRight);
                selectedRight->Flags |= TEST_OPEN_FLAG;
                selectedRight = nullptr;
            }
#if HAVE_MINIGUI
            if (selectedRight && (selectedRight == root || MiniGUI::Window::Cast(selectedRight) != nullptr) && ImGui::Button("Add Window"))
            {
                update = true;
                AddWindow(selectedRight);
                selectedRight->Flags |= TEST_OPEN_FLAG;
                selectedRight = nullptr;
            }
#endif
            if (selectedRight && selectedRight->GetParent() && ImGui::Button("Remove Node"))
            {
                update = true;
                if (selectedLeft == selectedRight)
                {
                    selectedLeft = nullptr;
                    Inspector::Select(nullptr);
                    Scene::Select(nullptr);
                }
                selectedRight->GetParent()->DetachChild(selectedRight);
                selectedRight = nullptr;
            }
            ImGui::Separator();
            if (ImGui::Button("Import Object"))
            {
                update = true;
                importNode = selectedRight;
                selectedRight = nullptr;
            }
            if (ImGui::Button("Export Object"))
            {
                update = true;
                Event::Push(ExportEvent::Create(selectedRight));
//              exportNode = selectedRight;
                selectedRight = nullptr;
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();

#if HAVE_MINIGUI
    auto& window = MiniGUI::Window::Cast(selectedLeft);
    if (window)
    {
        MiniGUI::WindowPtr const& parent = NodeTools::GetRoot(window);
        MiniGUI::Window::Traversal(parent, [&](MiniGUI::WindowPtr const& window)
        {
            if (window != selectedLeft)
                Tools::Rect(window->GetWorldOffset(), window->GetWorldOffset() + window->GetWorldScale(), 0xFF3F3F3F);
            return true;
        });
        Tools::Rect(window->GetWorldOffset(), window->GetWorldOffset() + window->GetWorldScale());
    }
#endif
    if (hovered)
    {
#if HAVE_MINIGUI
        auto& window = MiniGUI::Window::Cast(hovered);
        if (window)
        {
            Tools::Rect(window->GetWorldOffset(), window->GetWorldOffset() + window->GetWorldScale());
        }
        else
#endif
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
            {
                Node::Traversal(hovered, [&](xxNodePtr const& node)
                {
                    xxVector4 const& bound = node->WorldBound;
                    if (bound.radius != 0.0f)
                    {
                        Tools::Sphere(bound.xyz, bound.radius);
                    }
                    return true;
                });
            }
            else
            {
                Tools::Sphere(hovered->WorldBound.xyz, hovered->WorldBound.radius);
            }
        }
    }

    Import(updateData);
    Export(updateData);

    return update;
}
//==============================================================================
