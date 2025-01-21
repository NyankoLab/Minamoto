//==============================================================================
// Minamoto : DrawTools Source
//
// Copyright (c) 2019-2024 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Runtime.h"
#include <xxGraphicPlus/xxCamera.h>
#include <xxGraphicPlus/xxMesh.h>
#include <xxGraphicPlus/xxNode.h>
#if HAVE_MINIGUI
#include <MiniGUI/Window.h>
#endif
#include "DrawTools.h"

//==============================================================================
void DrawTools::Cull(xxNodePtr const& node, xxCameraPtr const& camera, std::vector<xxNode*>& array, bool append)
{
    if (append == false)
        array.clear();

    xxMatrix4x2 frustum[6];
    if (camera)
    {
        camera->GetFrustumPlanes(frustum[0], frustum[1], frustum[2], frustum[3], frustum[4], frustum[5]);
    }

    CullTraversal(node, frustum, array, camera ? -1 : 0);
}
//------------------------------------------------------------------------------
void DrawTools::Draw(DrawData& drawData, xxNodePtr const& node)
{
    xxMatrix4x2 frustum[6];
    if (drawData.camera3D)
    {
        drawData.camera = drawData.camera3D.get();
        if (drawData.frustum == nullptr)
        {
            drawData.frustum = frustum;
            drawData.camera3D->GetFrustumPlanes(frustum[0], frustum[1], frustum[2], frustum[3], frustum[4], frustum[5]);
        }
    }

    if (node->Mesh)
        node->Draw(drawData);

    for (xxNodePtr const& child : (*node))
    {
#if HAVE_MINIGUI
        auto window = MiniGUI::Window::Cast(child);
        if (window)
        {
            xxCamera* camera = drawData.camera;
            drawData.camera = drawData.camera2D.get();
            DrawTraversal(drawData, window);
            drawData.camera = camera;
            continue;
        }
#endif
        DrawTraversal(drawData, child);
    }
}
//------------------------------------------------------------------------------
void DrawTools::CullTraversal(xxNodePtr const& node, xxMatrix4x2 const frustum[6], std::vector<xxNode*>& array, char planes)
{
    if (planes)
    {
        for (int i = 0; i < 6; ++i)
        {
            char bit = (1 << i);
            if (planes & bit)
            {
                xxVector4 bound = node->WorldBound;
                float direction = frustum[i][0].xyz.Dot(bound.xyz - frustum[i][1].xyz);
                if (direction < -bound.w)
                    return;
                if (direction > bound.w)
                    planes &= ~bit;
            }
        }
    }

    if (node->Mesh)
    {
        array.push_back(node.get());
        return;
    }

    for (xxNodePtr const& child : (*node))
    {
#if HAVE_MINIGUI
        auto window = MiniGUI::Window::Cast(child);
        if (window)
        {
            continue;
        }
#endif
        CullTraversal(child, frustum, array, planes);
    }
}
//------------------------------------------------------------------------------
void DrawTools::DrawTraversal(DrawData& drawData, xxNodePtr const& node)
{
    if (node == nullptr)
        return;
    if (node->Mesh)
        node->Draw(drawData);
    for (xxNodePtr const& child : (*node))
        DrawTraversal(drawData, child);
}
//==============================================================================
