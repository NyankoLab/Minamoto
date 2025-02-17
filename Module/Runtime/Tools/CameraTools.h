//==============================================================================
// Minamoto : CameraTools Header
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#pragma once

#include "Runtime.h"

struct RuntimeAPI CameraTools
{
    static void MoveArcball(xxVector3& offset, xxVector3& arcball, float x, float y);
    static bool MoveCamera(xxCameraPtr const& camera, float elapsed, float forward_backward, float left_right, float up_down, float x, float y);
    static void SetViewport(xxCameraPtr const& camera, float fromWidth, float fromHeight, float toX, float toY, float toWidth, float toHeight);
    static xxVector3 GetDirectionFromScreenPos(xxCameraPtr const& camera, float x, float y);
    static xxVector3 GetScreenPosToWorldPos(xxCameraPtr const& camera, xxVector3 const& point);
    static xxVector4 GetWorldPosToScreenPos(xxCameraPtr const& camera, xxVector3 const& point);
};
