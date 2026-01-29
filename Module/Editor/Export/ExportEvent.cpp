//==============================================================================
// Minamoto : ExportEvent Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxFile.h>
#include <Runtime/Graphic/Binary.h>
#include <Runtime/Graphic/Node.h>
#include <Runtime/Tools/NodeTools.h>
#include "ExportFilmbox.h"
#include "ExportWavefront.h"
#include "ExportEvent.h"

#define TAG "ExportEvent"

//==============================================================================
//  ExportEvent
//==============================================================================
ExportEvent::ExportEvent(xxNodePtr const& root)
{
    static unsigned int accum = 0;
    this->title = "Export " + root->Name + " (" + std::to_string(accum++) + ")";
    this->root = root;
#if defined(_WIN32)
    this->path = std::string(xxGetDocumentPath()) + '\\';
#else
    this->path = std::string(xxGetDocumentPath()) + '/';
#endif
    this->name = root->Name;
}
//------------------------------------------------------------------------------
ExportEvent::~ExportEvent()
{
}
//------------------------------------------------------------------------------
double ExportEvent::Execute()
{
    bool show = true;
    if (ImGui::Begin(title.c_str(), &show, ImGuiWindowFlags_NoDocking))
    {
        if (pathItemWidth == false)
        {
            pathItemWidth = true;
            ImGui::SetNextItemWidth(512.0f);
        }
        ImGui::InputTextEx("Path", nullptr, path);
        ImGui::SameLine();
        if (ImGui::Button("..."))
        {
        }
        ImGui::InputTextEx("Name", nullptr, name);
        ImGui::BeginTabBar("Export");
        if (ImGui::BeginTabItem("Filmbox"))
        {
            if (ImGui::Button("Export"))
                Filmbox();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Minamoto"))
        {
            if (ImGui::Button("Export"))
                Minamoto();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Wavefront"))
        {
            if (ImGui::Button("Export"))
                Wavefront();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }
    ImGui::End();

    if (show)
    {
        double time;
        xxGetCurrentTime(&time);
        return time;
    }
    return 0.0;
}
//------------------------------------------------------------------------------
void ExportEvent::Filmbox()
{
    std::string filename = path + '/' + name + ".fbx";

    float begin = xxGetCurrentTime();
    if (ExportFilmbox::Save(filename.c_str(), root))
    {
        xxLog(TAG, "Export : %s (%0.fus)", xxFile::GetName(filename.c_str(), true).c_str(), (xxGetCurrentTime() - begin) * 1000000);
    }
}
//------------------------------------------------------------------------------
void ExportEvent::Minamoto()
{
    std::string filename = path + '/' + name + ".xxb";

    Node::Traversal(root, [&](xxNodePtr const& node)
    {
        node->Flags &= ~NodeTools::TEST_CHECK_FLAG;
        return true;
    });

    float begin = xxGetCurrentTime();
    if (Binary::Save(filename.c_str(), root))
    {
        xxLog(TAG, "Export : %s (%0.fus)", xxFile::GetName(filename.c_str(), true).c_str(), (xxGetCurrentTime() - begin) * 1000000);
    }
}
//------------------------------------------------------------------------------
void ExportEvent::Wavefront()
{
    std::string filename = path + '/' + name + ".obj";

    float begin = xxGetCurrentTime();
    if (ExportWavefront::Save(filename.c_str(), root))
    {
        xxLog(TAG, "Export : %s (%0.fus)", xxFile::GetName(filename.c_str(), true).c_str(), (xxGetCurrentTime() - begin) * 1000000);
    }
}
//------------------------------------------------------------------------------
std::shared_ptr<ExportEvent> ExportEvent::Create(xxNodePtr const& root)
{
    return std::make_shared<ExportEvent>(root);;
}
//------------------------------------------------------------------------------
