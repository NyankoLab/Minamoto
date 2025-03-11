//==============================================================================
// Minamoto : DrawTools Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

struct RuntimeAPI DrawTools
{
    struct DrawData : public xxDrawData
    {
        xxCameraPtr     camera2D;
        xxCameraPtr     camera3D;
    };

    static void Cull(xxNodePtr const& node, xxCameraPtr const& camera, std::vector<Node*>& scene, std::vector<Node*>* gui = nullptr, bool append = false);
    static void Draw(DrawData& drawData, xxNodePtr const& node);
protected:
    static void CullTraversal(xxNodePtr const& node, xxMatrix4x2 const frustum[6], std::vector<Node*>& array, char planes);
    static void DrawTraversal(DrawData& drawData, xxNodePtr const& node);
};
