//==============================================================================
// Minamoto : Camera Source
//
// Copyright (c) 2023-2026 TAiGA
// https://github.com/NyankoLab/Minamoto
//==============================================================================
#include "Runtime.h"
#include "Camera.h"

//------------------------------------------------------------------------------
static xxCameraPtr (*backupBinaryCreate)();
//------------------------------------------------------------------------------
void Camera::Initialize()
{
    if (backupBinaryCreate)
        return;
    backupBinaryCreate = xxCamera::BinaryCreate;

    xxCamera::BinaryCreate = []() -> xxCameraPtr
    {
        xxCameraPtr camera = xxCameraPtr(new Camera(), [](Camera* camera) { delete camera; });
        if (camera == nullptr)
            return nullptr;

        return camera;
    };
}
//------------------------------------------------------------------------------
void Camera::Shutdown()
{
    if (backupBinaryCreate == nullptr)
        return;
    xxCamera::BinaryCreate = backupBinaryCreate;
    backupBinaryCreate = nullptr;
}
//------------------------------------------------------------------------------
