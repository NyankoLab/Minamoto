//==============================================================================
// Minamoto : Camera Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
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
