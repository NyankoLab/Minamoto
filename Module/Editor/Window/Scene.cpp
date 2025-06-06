//==============================================================================
// Minamoto : Scene Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <xxGraphicPlus/xxModifier.h>
#include <xxGraphicPlus/xxTexture.h>
#include <Runtime/Graphic/Camera.h>
#include <Runtime/Graphic/Material.h>
#include <Runtime/Graphic/Mesh.h>
#include <Runtime/Graphic/Node.h>
#include <ImGuizmo/ImGuizmo.cpp>
#include <Tools/CameraTools.h>
#include <Tools/DrawTools.h>
#include <Tools/NodeTools.h>
#if HAVE_MINIGUI
#include <MiniGUI/Window.h>
#endif
#include "Utility/Grid.h"
#include "Utility/Tools.h"
#include "Hierarchy.h"
#include "Inspector.h"
#include "Profiler.h"
#include "Scene.h"

//==============================================================================
xxCameraPtr Scene::screenCamera;
xxCameraPtr Scene::sceneCamera;
xxCameraPtr Scene::mainCamera;
xxNodePtr Scene::sceneRoot;
xxNodePtr Scene::selected;
xxNodePtr Scene::selecting;
//------------------------------------------------------------------------------
static xxVector3 sceneArcball = {0.85f, -M_PI_2, 14.0f};
static xxVector3 mainCameraOffset = {1.0f, 0.0f, 0.0f};
static xxNodePtr sceneGrid;
static xxVector2 viewPos;
static xxVector2 viewSize;
static ImGuiViewport* viewViewport;
static std::vector<Node*> drawScenes;
static std::vector<Node*> drawGUIs;
static bool cullEnabled = false;
static bool drawBoneLine = false;
static bool drawNodeLine = false;
static bool drawNodeBound = false;
static bool manipulateHovered = false;
//------------------------------------------------------------------------------
void Scene::Initialize()
{
    if (mainCamera == nullptr)
    {
        mainCamera = xxCamera::Create();
        CameraTools::MoveArcball(mainCameraOffset, sceneArcball, 0, 0);
        mainCamera->Location = (xxVector3::Y * -10 + xxVector3::Z * 10);
        mainCamera->LookAt(xxVector3::ZERO, xxVector3::Z);
        mainCamera->Update();

        mainCamera->LightColor = {1.0f, 1.0f, 1.0f};
        mainCamera->LightDirection = xxVector3::Z;
    }
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

    if (sceneRoot == nullptr)
        sceneRoot = xxNode::Create();
    if (sceneGrid == nullptr)
        sceneGrid = Grid::Create(xxVector3::ZERO, {10000, 10000});
}
//------------------------------------------------------------------------------
void Scene::Shutdown(bool suspend)
{
    auto invalidate = [](xxNodePtr const& node)
    {
        node->Invalidate();
        if (node->Mesh)
            node->Mesh->Invalidate();
        if (node->Material)
        {
            node->Material->Invalidate();
            for (xxTexturePtr const& texture : node->Material->Textures)
            {
                if (texture->Path.empty() == false)
                    texture->Initialize(0, 0, 0, 0, 0, 0);
                texture->Invalidate();
            }
        }
        return true;
    };

    Node::Traversal(sceneRoot, invalidate);
    Node::Traversal(sceneGrid, invalidate);
    Node::Traversal(selected, invalidate);

    if (suspend)
        return;

    screenCamera = nullptr;
    mainCamera = nullptr;
    sceneRoot = nullptr;
    sceneGrid = nullptr;
    selected = nullptr;
    selecting = nullptr;
    viewViewport = nullptr;
    drawScenes = std::vector<Node*>();
    drawGUIs = std::vector<Node*>();
}
//------------------------------------------------------------------------------
void Scene::Select(xxNodePtr const& node)
{
    selected = node;
}
//------------------------------------------------------------------------------
void Scene::DrawBoneLine(xxNodePtr const& root)
{
    if (drawBoneLine)
    {
        Node::Traversal(root, [](xxNodePtr const& node)
        {
            for (auto const& data : node->Bones)
            {
                if (data.bone.use_count())
                {
                    xxNodePtr const& bone = (xxNodePtr&)data.bone;
                    xxNodePtr const& parent = bone->GetParent();
                    if (parent)
                    {
                        Tools::Line(parent->GetWorldTranslate(), bone->GetWorldTranslate());
                    }
                    xxVector3 direction = (xxVector4{ 0, 1, 0, 1 } * bone->WorldMatrix).xyz;
                    Tools::Line(bone->GetWorldTranslate(), direction);
                }
            }
            return true;
        });
    }
}
//------------------------------------------------------------------------------
void Scene::DrawCameraLine(const UpdateData& updateData, xxCameraPtr const& camera)
{
    if (camera)
    {
        float frustumNear = camera->FrustumNear;
        float frustumFar = std::min(camera->FrustumFar, 10.0f);
        xxVector3 const& location = camera->Location;
        xxVector3 near[4] =
        {
            { 0.0f, 0.0f, frustumNear },
            { 0.0f, 1.0f, frustumNear },
            { 1.0f, 1.0f, frustumNear },
            { 1.0f, 0.0f, frustumNear },
        };
        xxVector3 far[4] =
        {
            { 0.0f, 0.0f, frustumFar },
            { 0.0f, 1.0f, frustumFar },
            { 1.0f, 1.0f, frustumFar },
            { 1.0f, 0.0f, frustumFar },
        };
        for (int i = 0; i < 4; ++i)
        {
            near[i] = CameraTools::GetScreenPosToWorldPos(camera, near[i]);
            far[i] = CameraTools::GetScreenPosToWorldPos(camera, far[i]);
        }
        for (int i = 0; i < 4; ++i)
        {
            Tools::Line(location, near[i]);
            Tools::Line(near[i], far[i]);
            Tools::Line(near[i], near[(i + 1) % 4]);
            Tools::Line(far[i], far[(i + 1) % 4]);
        }
    }
}
//------------------------------------------------------------------------------
void Scene::DrawNodeLine(xxNodePtr const& root)
{
    if (drawNodeLine)
    {
        Node::Traversal(root, [](xxNodePtr const& node)
        {
            xxNodePtr const& parent = node->GetParent();
            if (parent)
            {
                Tools::Line(parent->GetWorldTranslate(), node->GetWorldTranslate());
            }
            return true;
        });
    }
}
//------------------------------------------------------------------------------
void Scene::DrawNodeBound(xxNodePtr const& root)
{
    if (drawNodeBound)
    {
        Node::Traversal(root, [](xxNodePtr const& node)
        {
            xxVector4 const& bound = node->WorldBound;
            if (bound.radius != 0.0f)
            {
                Tools::Sphere(bound.xyz, bound.radius);
            }
            return true;
        });
    }
}
//------------------------------------------------------------------------------
static void PreSelectMouse()
{
    Scene::selecting = nullptr;

    if (manipulateHovered)
        return;

    if (ImGui::IsMouseHoveringRect(ImVec2(viewPos.x, viewPos.y), ImVec2(viewPos.x + viewSize.x, viewPos.y + viewSize.y)))
    {
        xxVector2 mousePos = { ImGui::GetMousePos().x, ImGui::GetMousePos().y };
        xxVector2 point = (mousePos - viewPos) / viewSize;
        xxVector3 direction = CameraTools::GetDirectionFromScreenPos(Scene::mainCamera, point.x, point.y);
        xxNodePtr const& node = NodeTools::Intersect(Scene::sceneRoot, Scene::mainCamera->Location, direction);
        if (node)
        {
            if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
            {
                Hierarchy::Select(node);
                Inspector::Select(node);
                Scene::Select(node);
            }
            else
            {
                xxVector4 const& bound = node->WorldBound;
                if (bound.radius != 0.0f)
                {
                    Tools::Sphere(bound.xyz, bound.radius);
                }

                Scene::selecting = node;
            }
        }
    }
}
//------------------------------------------------------------------------------
static void SelectMouse()
{
    manipulateHovered = false;

    xxNodePtr const& selected = Scene::selected;
    if (selected)
    {
        ImGuizmo::gContext.mbOverGizmoHotspot = false;
        ImGuizmo::SetRect(viewPos.x, viewPos.y, viewSize.x, viewSize.y);
        if (selected->Camera)
        {
            static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE_X | ImGuizmo::ROTATE_Z;
            static ImGuizmo::MODE mode = ImGuizmo::WORLD;
            xxMatrix4 camera;
            camera[0].xyz = selected->Camera->Up;
            camera[1].xyz = selected->Camera->Right;
            camera[2].xyz = selected->Camera->Direction;
            camera[3].xyz = selected->Camera->Location;
            camera[3].w = 1.0f;
            ImGuizmo::Manipulate(Scene::mainCamera->ViewMatrix, Scene::mainCamera->ProjectionMatrix, operation, mode, camera);
            selected->Camera->Up = camera[0].xyz;
            selected->Camera->Right = camera[1].xyz;
            selected->Camera->Direction = camera[2].xyz;
            selected->Camera->Location = camera[3].xyz;
            selected->Camera->Update();
        }
        else
        {
            static ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE | ImGuizmo::ROTATE;
            static ImGuizmo::MODE mode = ImGuizmo::WORLD;
            xxMatrix4 world = selected->WorldMatrix;
            if (ImGuizmo::Manipulate(Scene::mainCamera->ViewMatrix, Scene::mainCamera->ProjectionMatrix, operation, mode, world))
            {
                xxNodePtr const& parent = selected->GetParent();
                if (parent)
                {
                    selected->LocalMatrix = parent->WorldMatrix.Inverse() * world;
                }
                else
                {
                    selected->LocalMatrix = world;
                }
                selected->UpdateMatrix();
            }
        }

        manipulateHovered = ImGuizmo::IsOver();
    }
}
//------------------------------------------------------------------------------
static bool CameraMoveWASD(const UpdateData& updateData, bool mani)
{
    ImGuiIO& io = ImGui::GetIO();

    float forward_backward = 0.0f;
    float left_right = 0.0f;
    float speed = 10.0f;
    if (ImGui::IsWindowFocused())
    {
        if (ImGui::IsKeyDown(ImGuiKey_Escape))
        {
            Hierarchy::Select(nullptr);
            Inspector::Select(nullptr);
            Scene::Select(nullptr);
        }
        if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
        {
            speed = 100.0f;
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
        if (ImGui::IsKeyPressed(ImGuiKey_Z) && Scene::mainCamera)
        {
            if (Scene::selected)
            {
                Tools::LookAtFromBound(Scene::mainCamera, Scene::selected->WorldBound, xxVector3::Z);
            }
            else if (Scene::sceneRoot)
            {
                Tools::LookAtFromBound(Scene::mainCamera, Scene::sceneRoot->WorldBound, xxVector3::Z);
            }
        }
    }

    static xxVector2 mouse = xxVector2::ZERO;
    static bool pressed = false;
    float x = 0.0f;
    float y = 0.0f;
    if (pressed || (ImGui::IsWindowHovered() && mani == false))
    {
        if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
        {
            // ImGui::GetCurrentWindow() failed on MSVC
            ImGuiContext* context = ImGui::GetCurrentContext();
            ImGui::FocusWindow(context->CurrentWindow);
            x = (io.MousePos.x - mouse.x) / 1000.0f * updateData.scale;
            y = (io.MousePos.y - mouse.y) / 1000.0f * (16.0f / 9.0f) * updateData.scale;
            pressed = true;
        }
        else
        {
            pressed = false;
        }
    }
    mouse.x = io.MousePos.x;
    mouse.y = io.MousePos.y;

    if (forward_backward || left_right || x || y)
    {
        CameraTools::MoveCamera(Scene::mainCamera, updateData.elapsed, forward_backward, left_right, 0.0f, x, y);
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
static bool CameraMoveManipulate(bool mani, ImVec2 const& maniSize, ImVec2 const& maniPos)
{
    xxMatrix4 identityMatrix = xxMatrix4::IDENTITY;
    xxMatrix4 viewMatrix = Scene::mainCamera->ViewMatrix;

    ImGuizmo::SetDrawlist();
    ImGuizmo::SetRect(viewPos.x, viewPos.y, viewSize.x, viewSize.y);
    ImGuizmo::ViewManipulate(viewMatrix, Scene::mainCamera->ProjectionMatrix, ImGuizmo::ROTATE, ImGuizmo::WORLD, identityMatrix, 8.0f, maniPos, maniSize, 0x10101010);
    if (mani)
    {
        Scene::mainCamera->Right = { viewMatrix[0].x, viewMatrix[1].x, viewMatrix[2].x };
        Scene::mainCamera->Up = { viewMatrix[0].y, viewMatrix[1].y, viewMatrix[2].y };
        Scene::mainCamera->Direction = { viewMatrix[0].z, viewMatrix[1].z, viewMatrix[2].z };
        return true;
    }

    return false;
}
//------------------------------------------------------------------------------
#if HAVE_MINIGUI
static void MiniGUIEditor(MiniGUI::WindowPtr const& window)
{
    if (window == nullptr)
        return;

    static int dragType = 0;
    static bool dragging = false;
    constexpr int LT = 0;
    constexpr int RT = 1;
    constexpr int LB = 2;
    constexpr int RB = 3;

    xxVector2 mousePos;
    mousePos.x = ImGui::GetIO().MousePos.x;
    mousePos.y = ImGui::GetIO().MousePos.y;

    // Drag
    if (dragging)
    {
        xxVector2 lt = window->GetOffset();
        xxVector2 rb = window->GetOffset() + window->GetScale();
        xxVector2 pt = (mousePos - viewPos) / viewSize;
        MiniGUI::WindowPtr parent = window->GetParent();
        if (parent)
        {
            pt = (pt - parent->GetWorldOffset()) / parent->GetWorldScale();
        }
        switch (dragType)
        {
        case LT:
            window->SetScale({ rb.x - pt.x, rb.y - pt.y});
            window->SetOffset({ pt.x, pt.y });
            break;
        case RT:
            window->SetScale({ pt.x - lt.x, rb.y - pt.y});
            window->SetOffset({ lt.x, pt.y });
            break;
        case LB:
            window->SetScale({ rb.x - pt.x, pt.y - lt.y});
            window->SetOffset({ pt.x, lt.y });
            break;
        case RB:
            window->SetScale({ pt.x - lt.x, pt.y - lt.y});
            window->SetOffset({ lt.x, lt.y });
            break;
        default:
            break;
        }
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
        {
            dragging = false;
        }
        return;
    }

    // Edge
    xxVector2 lt = window->GetWorldOffset();
    xxVector2 rb = window->GetWorldOffset() + window->GetWorldScale();
    lt = lt * viewSize + viewPos;
    rb = rb * viewSize + viewPos;
    bool hitX0 = std::fabs(mousePos.x - lt.x) < 8.0f;
    bool hitY0 = std::fabs(mousePos.y - lt.y) < 8.0f;
    bool hitX1 = std::fabs(mousePos.x - rb.x) < 8.0f;
    bool hitY1 = std::fabs(mousePos.y - rb.y) < 8.0f;
    if (hitX0 || hitX1 || hitY0 || hitY1)
    {
        int type = -1;
        ImDrawList* drawList = ImGui::GetWindowDrawList();
        if (hitX0 && hitY0)
        {
            type = LT;
            drawList->AddCircle({ lt.x, lt.y }, 8.0f, 0xFFFFFFFF);
        }
        else if (hitX1 && hitY0)
        {
            type = RT;
            drawList->AddCircle({ rb.x, lt.y }, 8.0f, 0xFFFFFFFF);
        }
        else if (hitX0 && hitY1)
        {
            type = LB;
            drawList->AddCircle({ lt.x, rb.y }, 8.0f, 0xFFFFFFFF);
        }
        else if (hitX1 && hitY1)
        {
            type = RB;
            drawList->AddCircle({ rb.x, rb.y }, 8.0f, 0xFFFFFFFF);
        }
        if (type >= LT && type <= RB)
        {
            if (ImGui::IsMouseDown(ImGuiMouseButton_Left))
            {
                dragType = type;
                dragging = true;
                return;
            }
        }
    }

    // Hit
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        MiniGUI::WindowPtr root = NodeTools::GetRoot(window);
        MiniGUI::WindowPtr selected = window;
        auto callback = [&](MiniGUI::WindowPtr const& window)
        {
            xxVector2 lt = window->GetWorldOffset();
            xxVector2 rb = window->GetWorldOffset() + window->GetWorldScale();
            lt = lt * viewSize + viewPos;
            rb = rb * viewSize + viewPos;
            if (mousePos.x >= lt.x && mousePos.x <= rb.x && mousePos.y >= lt.y && mousePos.y <= rb.y)
            {
                Hierarchy::Select(window);
                Inspector::Select(window);
                Scene::Select(window);
            }
            return true;
        };
        MiniGUI::Window::Traversal(root, callback);
    }
}
#endif
//------------------------------------------------------------------------------
bool Scene::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    bool updated = false;
    if (sceneRoot)
    {
        NodeTools::UpdateNodeFlags(sceneRoot);

        // Count
        struct
        {
            size_t bone;
            size_t nodeTotal;
            size_t nodeActive;
            size_t modifierTotal;
            size_t modifierActive;
        } Count = {};
        auto callback = [&](xxNodePtr const& node)
        {
            node->Flags &= ~NodeTools::TEST_CHECK_FLAG;
            for (auto const& data : node->Bones)
            {
                if (data.bone.use_count())
                {
                    xxNodePtr const& bone = (xxNodePtr&)data.bone;
                    if ((bone->Flags & NodeTools::TEST_CHECK_FLAG) == 0)
                    {
                        bone->Flags |= NodeTools::TEST_CHECK_FLAG;
                        Count.bone++;
                    }
                }
            }
            Count.nodeTotal++;
            Count.modifierTotal += node->Modifiers.size();
            if ((node->Flags & xxNode::UPDATE_SKIP) == 0)
            {
                Count.nodeActive++;
                Count.modifierActive += node->Modifiers.size();
            }
            return true;
        };
        for (xxNodePtr const& node : (*sceneRoot))
        {
#if HAVE_MINIGUI
            if (MiniGUI::Window::Cast(node))
                continue;
#endif
            Node::Traversal(node, callback);
        }
        Profiler::Count(xxHash("Bone Count"), Count.bone);
        Profiler::Count(xxHash("Node Total Count"), Count.nodeTotal);
        Profiler::Count(xxHash("Node Active Count"), Count.nodeActive);
        Profiler::Count(xxHash("Modifier Total Count"), Count.modifierTotal);
        Profiler::Count(xxHash("Modifier Active Count"), Count.modifierActive);
        updated |= Count.modifierTotal != 0;

        // Scene
        Profiler::Begin(xxHash("Scene Update"));
        for (xxNodePtr const& node : (*sceneRoot))
        {
#if HAVE_MINIGUI
            if (MiniGUI::Window::Cast(node))
                continue;
#endif
            node->Update(updateData.time);
        }
        sceneRoot->UpdateBound();
        Profiler::End(xxHash("Scene Update"));

#if HAVE_MINIGUI
        // MiniGUI
        Profiler::Begin(xxHash("MiniGUI Update"));
        for (xxNodePtr const& node : (*sceneRoot))
        {
            auto& window = MiniGUI::Window::Cast(node);
            if (window)
            {
                MiniGUI::Window::Update(window, updateData.time, viewSize);
            }
        }
        Profiler::End(xxHash("MiniGUI Update"));
#endif

        if (show)
        {
            DrawBoneLine(sceneRoot);
            DrawNodeLine(sceneRoot);
            DrawNodeBound(sceneRoot);

//          if (selected)
//          {
//              DrawCameraLine(updateData, selected->Camera);
//          }
        }
    }

    if (ImGui::Begin(ICON_FA_GLOBE "Scene", &show))
    {
        ImGui::Checkbox("##1", &cullEnabled);   if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Culling");
        ImGui::SameLine();
        ImGui::Checkbox("##2", &drawBoneLine);  if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Draw Bone Line");
        ImGui::SameLine();
        ImGui::Checkbox("##3", &drawNodeLine);  if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Draw Node Line");
        ImGui::SameLine();
        ImGui::Checkbox("##4", &drawNodeBound); if (ImGui::IsItemHovered()) ImGui::SetTooltip("%s", "Draw Node Bound");

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

        ImDrawList* drawList = ImGui::GetWindowDrawList();
#if defined(xxWINDOWS)
        drawList->AddRectFilled({ viewPos.x, viewPos.y }, { viewPos.x + viewSize.x, viewPos.y + viewSize.y }, 0xFF778899);
#else
        drawList->AddRectFilled({ viewPos.x, viewPos.y }, { viewPos.x + viewSize.x, viewPos.y + viewSize.y }, 0xFF998877);
#endif
        drawList->AddCallback(Callback, (void*)&updateData, sizeof(updateData));
        drawList->AddCallback(ImDrawCallback_ResetRenderState, nullptr);

        PreSelectMouse();

        Tools::Draw(mainCamera, viewSize, viewPos);

#if HAVE_MINIGUI
        MiniGUIEditor(MiniGUI::Window::Cast(selected));
#endif

        ImGui::Text("Screen : %.0f x %.0f", viewSize.x, viewSize.y);
        ImGui::Text("Scene : %zd", drawScenes.size());
        ImGui::Text("GUI : %zd", drawGUIs.size());

        // Manipulate
        SelectMouse();

        // ViewManipulate
        ImVec2 maniSize = ImVec2(128, 128);
        ImVec2 maniPos = ImVec2(viewPos.x + viewSize.x - maniSize.x, viewPos.y);
        bool mani = ImGui::IsMouseHoveringRect(maniPos, { maniPos.x + maniSize.x, maniPos.y + maniSize.y });

        updated |= CameraMoveWASD(updateData, mani);
        updated |= CameraMoveManipulate(mani, maniSize, maniPos);

        DrawTools::Cull(sceneRoot, cullEnabled ? sceneCamera : mainCamera, drawScenes, &drawGUIs, false);
    }
    ImGui::End();

    return updated;
}
//------------------------------------------------------------------------------
void Scene::Callback(const ImDrawList* list, const ImDrawCmd* cmd)
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

    mainCamera->SetFOV(viewport_width / viewport_height, 60.0f, 10000.0f);
    mainCamera->Update();

    const UpdateData* updateData = (UpdateData*)cmd->UserCallbackData;

    DrawTools::DrawData drawData;
    drawData.device = updateData->device;
    drawData.commandEncoder = commandEncoder;
    drawData.camera2D = screenCamera;
    drawData.camera3D = mainCamera;
    drawData.materialIndex = Material::DEFAULT;

    xxMatrix4x2 frustum[6];
    if (sceneCamera)
    {
        drawData.frustum = frustum;
        sceneCamera->GetFrustumPlanes(frustum[0], frustum[1], frustum[2], frustum[3], frustum[4], frustum[5]);
    }

    Profiler::Begin(xxHash("Scene Render"));
    drawData.camera = drawData.camera3D.get();
    for (Node* node : drawScenes)
    {
        node->Draw(drawData);
    }
    Profiler::End(xxHash("Scene Render"));

    DrawTools::Draw(drawData, sceneGrid);

    if (selecting)
    {
        drawData.materialIndex = Material::SELECT;
        if (selecting && selecting->Mesh)
            selecting->Draw(drawData);
        drawData.materialIndex = Material::DEFAULT;
    }

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
