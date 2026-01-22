//==============================================================================
// Minamoto : Log Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Editor.h"
#include <queue>
#include "Log.h"

static std::deque<char*> systemLog;
//------------------------------------------------------------------------------
void Log::Initialize()
{
    
}
//------------------------------------------------------------------------------
void Log::Shutdown()
{
    for (char* line : systemLog)
        xxFree(line);
    systemLog = std::deque<char*>();
}
//------------------------------------------------------------------------------
bool Log::StatusBar()
{
    bool update = false;

    ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
    ImGui::BeginViewportSideBar("##MainStatusBar", ImGui::GetMainViewport(), ImGuiDir_Down, ImGui::GetFrameHeight(), window_flags);
    if (ImGui::BeginMenuBar())
    {
        static size_t logCount = 0;
        if (logCount != systemLog.size())
        {
            logCount = systemLog.size();
            update = true;
        }
        if (systemLog.empty() == false)
        {
            ImGui::TextUnformatted(systemLog.back());
        }
        ImGui::EndMenuBar();
    }
    ImGui::End();

    return update;
}
//------------------------------------------------------------------------------
bool Log::Update(const UpdateData& updateData, bool& show)
{
    updateData.message({ "LOGGER_UPDATE", (char*)&systemLog });

    if (show == false)
    {
        return StatusBar();
    }

    if (ImGui::Begin(ICON_FA_DESKTOP "Log", &show))
    {
        if (systemLog.empty() == false)
        {
            ImGuiListClipper clipper;
            clipper.Begin((int)systemLog.size(), ImGui::GetTextLineHeightWithSpacing());
            while (clipper.Step())
            {
                auto start = systemLog.begin() + clipper.DisplayStart;
                auto end = systemLog.begin() + clipper.DisplayEnd;
                for (auto it = start; it != end; ++it)
                {
                    ImGui::Selectable(*it, false);
                }
            }
        }

        static size_t logCount = 0;
        if (logCount != systemLog.size())
        {
            logCount = systemLog.size();
            ImGui::TextUnformatted("");
            ImGui::SetScrollHereY();
        }
    }
    ImGui::End();

    return StatusBar();
}
//------------------------------------------------------------------------------
