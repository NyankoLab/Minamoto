//==============================================================================
// Minamoto : Game Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <Runtime/Graphic/Camera.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Node.h>
#include <Tools/CameraTools.h>
#include <Tools/DrawTools.h>
#if HAVE_MINIGUI
#include <MiniGUI/Window.h>
#endif
#include "Profiler.h"
#include "Scene.h"
#include "Game.h"

//==============================================================================
static xxCameraPtr screenCamera;
static xxCameraPtr sceneCamera;
static xxVector2 viewPos;
static xxVector2 viewSize;
static ImGuiViewport* viewViewport;
static std::vector<Node*> drawScenes;
static std::vector<Node*> drawGUIs;
//------------------------------------------------------------------------------
void Game::Initialize()
{
    if (screenCamera == nullptr)
    {
        screenCamera = xxCamera::Create();
        screenCamera->FrustumLeft = 0;
        screenCamera->FrustumRight = 1;
        screenCamera->FrustumTop = 0;
        screenCamera->FrustumBottom = 1;
        screenCamera->Up = xxVector3::Y;
        screenCamera->Right = xxVector3::X;
        screenCamera->Direction = xxVector3::Z;
        screenCamera->Location = xxVector3::ZERO;
        screenCamera->Update();
        screenCamera->ProjectionMatrix[3].x = -1.0f;
        screenCamera->ProjectionMatrix[3].y = 1.0f;
    }
}
//------------------------------------------------------------------------------
void Game::Shutdown(bool suspend)
{
    if (suspend)
        return;

    screenCamera = nullptr;
    viewViewport = nullptr;
    drawScenes = std::vector<Node*>();
    drawGUIs = std::vector<Node*>();
}
//------------------------------------------------------------------------------
bool Game::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    bool updated = false;
    if (ImGui::Begin(ICON_FA_GAMEPAD "Game", &show))
    {
        sceneCamera = nullptr;
        for (xxNodePtr const& node : (*Scene::sceneRoot))
        {
            if (node->Camera)
            {
                sceneCamera = node->Camera;
                break;
            }
        }

        ImVec2 pos = ImGui::GetWindowPos();
        ImVec2 size = ImGui::GetWindowSize();
        ImVec2 cursor = ImGui::GetCursorPos();
        ImVec2 framePadding = ImGui::GetStyle().FramePadding;
        viewPos.x = pos.x + cursor.x;
        viewPos.y = pos.y + cursor.y;
        viewSize.x = size.x - cursor.x - framePadding.x * 2.0f;
        viewSize.y = size.y - cursor.y - framePadding.y * 3.0f;
        ImGuiViewport* viewport = ImGui::GetWindowViewport();
        if (viewViewport != viewport)
        {
            viewViewport = viewport;
            updated = true;
        }

        float forward_backward = 0.0f;
        float left_right = 0.0f;
        float up_down = 0.0f;
        float speed = 10.0f;
        if (ImGui::IsWindowFocused())
        {
            if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
            {
                speed = 100.0f;
            }
            if (ImGui::IsKeyDown(ImGuiKey_Q))
            {
                up_down = -speed;
            }
            if (ImGui::IsKeyDown(ImGuiKey_E))
            {
                up_down = speed;
            }
            if (ImGui::IsKeyDown(ImGuiKey_A))
            {
                left_right = -speed;
            }
            if (ImGui::IsKeyDown(ImGuiKey_D))
            {
                left_right = speed;
            }
            if (ImGui::IsKeyDown(ImGuiKey_S))
            {
                forward_backward = -speed;
            }
            if (ImGui::IsKeyDown(ImGuiKey_W))
            {
                forward_backward = speed;
            }
        }

        if (sceneCamera && (forward_backward || left_right || up_down))
        {
            CameraTools::MoveCamera(sceneCamera, updateData.elapsed, forward_backward, left_right, up_down, 0.0f, 0.0f);
            updated = true;
        }

        ImDrawList* drawList = ImGui::GetWindowDrawList();
#if defined(xxWINDOWS)
        drawList->AddRectFilled({ viewPos.x, viewPos.y }, { viewPos.x + viewSize.x, viewPos.y + viewSize.y }, 0xFF778899);
#else
        drawList->AddRectFilled({ viewPos.x, viewPos.y }, { viewPos.x + viewSize.x, viewPos.y + viewSize.y }, 0xFF998877);
#endif
        drawList->AddCallback(Callback, (void*)&updateData, sizeof(updateData));
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

        DrawTools::Cull(Scene::sceneRoot, sceneCamera, drawScenes, &drawGUIs, false);
    }
    ImGui::End();

    return updated;
}
//------------------------------------------------------------------------------
void Game::Callback(const ImDrawList* list, const ImDrawCmd* cmd)
{
    if (viewViewport == nullptr || viewViewport->RendererUserData == nullptr)
        return;

    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    float dpiScale = 1.0f;
    uint64_t commandEncoder = 0;
    ImGuiPlatformIO& io = ImGui::GetPlatformIO();
    for (int i = 0; i < io.Viewports.size(); ++i)
    {
        ImGuiViewport* viewport = io.Viewports[i];
        if (viewport != viewViewport)
            continue;
        x = viewport->Pos.x;
        y = viewport->Pos.y;
        width = viewport->Size.x;
        height = viewport->Size.y;
#if defined(xxMACOS) || defined(xxIOS)
        dpiScale = viewport->DpiScale;
#endif
        commandEncoder = *(uint64_t*)viewport->RendererUserData;
        break;
    }
    if (commandEncoder == 0)
        return;

    float viewport_x = (viewPos.x - x);
    float viewport_y = (viewPos.y - y);
    float viewport_width = viewSize.x;
    float viewport_height = viewSize.y;
    if (viewport_x >= width || viewport_x + width < 0.0f || viewport_y >= height || viewport_y + height < 0.0f)
        return;

    viewport_x = std::max(viewport_x, 0.0f) * dpiScale;
    viewport_y = std::max(viewport_y, 0.0f) * dpiScale;
    viewport_width = std::min(viewport_width, width) * dpiScale;
    viewport_height = std::min(viewport_height, height) * dpiScale;

    xxSetViewport(commandEncoder, int(viewport_x), int(viewport_y), int(viewport_width), int(viewport_height), 0.0f, 1.0f);
    xxSetScissor(commandEncoder, int(viewport_x), int(viewport_y), int(viewport_width), int(viewport_height));

    const UpdateData* updateData = (UpdateData*)cmd->UserCallbackData;

    DrawTools::DrawData drawData;
    drawData.device = updateData->device;
    drawData.commandEncoder = commandEncoder;
    drawData.camera2D = screenCamera;
    drawData.camera3D = sceneCamera;
    drawData.materialIndex = 1;

    if (sceneCamera)
    {
        sceneCamera->SetFOV(viewport_width / viewport_height, 60.0f, 10000.0f);
        sceneCamera->Update();
    }

    Profiler::Begin(xxHash("Scene Render"));
    drawData.camera = drawData.camera3D.get();
    for (Node* node : drawScenes)
    {
        node->Draw(drawData);
    }
    Profiler::End(xxHash("Scene Render"));

#if HAVE_MINIGUI
    Profiler::Begin(xxHash("MiniGUI Render"));
    drawData.camera = drawData.camera2D.get();
    for (Node* node : drawGUIs)
    {
        node->Draw(drawData);
    }
    Profiler::End(xxHash("MiniGUI Render"));
#endif
}
//------------------------------------------------------------------------------
