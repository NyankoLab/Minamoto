//==============================================================================
// Minamoto : Validator Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include <Interface.h>

#include <xxGraphicPlus/xxFile.h>
#include <xxGraphicPlus/xxMath.h>
#include <xxGraphicPlus/xxNode.h>

#if DirectXMath
#include "DirectXMath.h"
using namespace DirectX;
#endif

#define PLUGIN_NAME     "Validator"
#define PLUGIN_MAJOR    1
#define PLUGIN_MINOR    0

static void ValidateFile(float time, char* text, size_t count);
static void ValidateNode(float time, char* text, size_t count);

//------------------------------------------------------------------------------
moduleAPI const char* Create(const CreateData& createData)
{
    return PLUGIN_NAME;
}
//------------------------------------------------------------------------------
moduleAPI void Shutdown(const ShutdownData& shutdownData)
{

}
//------------------------------------------------------------------------------
moduleAPI bool Update(const UpdateData& updateData)
{
    static bool showValidate = false;
    static bool showAbout = false;
    static bool showBoundIntersect = false;

    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu(PLUGIN_NAME))
        {
            ImGui::MenuItem("Validate", nullptr, &showValidate);
            ImGui::Separator();
            ImGui::MenuItem("Bound Intersect", nullptr, &showBoundIntersect);
            ImGui::Separator();
            ImGui::MenuItem("About " PLUGIN_NAME, nullptr, &showAbout);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showValidate)
    {
        ImGui::SetNextWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
        if (ImGui::Begin("Validate", &showValidate, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
        {
            static char text[4096];
            ImGui::InputTextMultiline("##source", text, IM_ARRAYSIZE(text), ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), ImGuiInputTextFlags_ReadOnly);

            if (ImGui::Button("File"))
            {
                ValidateFile(updateData.time, text, sizeof(text));
            }
            ImGui::SameLine();
            if (ImGui::Button("Node"))
            {
                ValidateNode(updateData.time, text, sizeof(text));
            }
        }
        ImGui::End();
    }

    if (showAbout)
    {
        if (ImGui::Begin("About " PLUGIN_NAME, &showAbout, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
        {
            ImGui::Text("%s Plugin Version %d.%d", PLUGIN_NAME, PLUGIN_MAJOR, PLUGIN_MINOR);
            ImGui::Text("Build Date : %s %s", __DATE__, __TIME__);
            ImGui::Separator();
            ImGui::DumpBuildInformation();
        }
        ImGui::End();
    }

    if (showBoundIntersect)
    {
        ImGui::SetNextWindowSize(ImVec2(512.0f, 384.0f));
        if (ImGui::Begin("Bound Intersect", &showBoundIntersect, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
        {
            ImDrawList* drawList = ImGui::GetWindowDrawList();
            ImVec2 pos = ImGui::GetWindowPos();

            static xxVector3 left = xxVector3{ 150, 200, 100.0f };
            static xxVector3 right = xxVector3{ 300, 200, 50.0f };

            static int mode = 0;
            ImGui::SliderFloat("Left", &left.x, 0, 500);
            if (ImGui::IsItemHovered())
                mode = 0;
            ImGui::SliderFloat("Right", &right.x, 0, 500);
            if (ImGui::IsItemHovered())
                mode = 1;

            ImU32 color = 0xFFFFFFFF;
            switch (mode == 0 ? left.Intersect(right) : right.Intersect(left))
            {
            case 0:
                color = 0xFFFF0000;
                break;
            case 1:
                color = 0xFF00FF00;
                break;
            case 2:
                color = 0xFF0000FF;
                break;
            }

            drawList->AddCircle(ImVec2(pos.x + left.x, pos.y + left.y), left.z, color);
            drawList->AddCircle(ImVec2(pos.x + right.x, pos.y + right.y), right.z, color);
        }
        ImGui::End();
    }

    return false;
}
//------------------------------------------------------------------------------
moduleAPI void Render(const RenderData& renderData)
{

}
//------------------------------------------------------------------------------
void ValidateFile(float time, char* text, size_t count)
{
    int step = 0;

    static char const* const names[] =
    {
        "foo",
        "foo/",
        "foo/bar",
        "foo/bar.123",
        "foo.123/bar",
        "foo.123/bar.123",
        "foo.123/bar.123/abc",
        "foo.123/bar.123/abc.123",
    };
    for (char const* name : names)
    {
        step += snprintf(text + step, count - step, "%s -> %s\n", name, xxFile::GetName(name).c_str());
    }
    for (char const* name : names)
    {
        step += snprintf(text + step, count - step, "%s -> %s\n", name, xxFile::GetPath(name).c_str());
    }
}
//------------------------------------------------------------------------------
void ValidateNode(float time, char* text, size_t count)
{
    int step = 0;

    // 1. Create Root
    xxNodePtr root = xxNode::Create();
    step += snprintf(text + step, count - step, "Root : %p\n", root.get());

    // 2. Create Child
    xxNodePtr child = xxNode::Create();
    step += snprintf(text + step, count - step, "Child : %p\n", child.get());

    // 3. Attach Child
    bool attachChild = root->AttachChild(child);
    step += snprintf(text + step, count - step, "Attach Child : %s\n", attachChild ? "TRUE" : "FALSE");

    // 4. Get Children Count
    step += snprintf(text + step, count - step, "Root Children Count : %zu\n", root->GetChildCount());
    for (size_t i = 0; i < root->GetChildCount(); ++i)
    {
        xxNodePtr const& node = root->GetChild(i);
        if (node != nullptr)
        {
            step += snprintf(text + step, count - step, "Children (%zu) : %p\n", i, node.get());
        }
    }

    // 5. Create GrandChild
    xxNodePtr grandChild = xxNode::Create();
    step += snprintf(text + step, count - step, "Child : %p\n", grandChild.get());

    // 6. Attach GrandChild
    bool attachGrandChild = child->AttachChild(grandChild);
    step += snprintf(text + step, count - step, "Attach Child : %s\n", attachGrandChild ? "TRUE" : "FALSE");

    // 7. Get Child's Children Count
    step += snprintf(text + step, count - step, "Child's Children Count : %zu\n", child->GetChildCount());
    for (size_t i = 0; i < child->GetChildCount(); ++i)
    {
        xxNodePtr const& node = child->GetChild(i);
        if (node != nullptr)
        {
            step += snprintf(text + step, count - step, "Children (%zu) : %p\n", i, node.get());
        }
    }

    // 8. Set Local Matrix
    root->LocalMatrix = { xxVector4::Z, -xxVector4::Y, xxVector4::X, xxVector4::WHITE };
    child->LocalMatrix = { xxVector4::Y, -xxVector4::X, xxVector4::Z, xxVector4::WHITE };
    child->SetRotate({ xxVector3::Y, -xxVector3::X, xxVector3::Z });
    child->SetTranslate(xxVector3::WHITE);
    child->SetScale(2.0f);
    child->UpdateRotateTranslateScale();
    grandChild->LocalMatrix = { xxVector4::X, -xxVector4::Z, xxVector4::Y, xxVector4::WHITE };
    step += snprintf(text + step, count - step, "SetLocalMatrix\n");

    // 9. Update
    root->Update(time);
    step += snprintf(text + step, count - step, "Update Root : %f\n", time);

    // 10. Get Root World Matrix
    for (int i = 0; i < 4; ++i)
    {
        const xxMatrix4& worldMatrix = root->WorldMatrix;
        step += snprintf(text + step, count - step, "Root Matrix (%u) : %f %f %f %f\n", i, worldMatrix.v[i].x, worldMatrix.v[i].y, worldMatrix.v[i].z, worldMatrix.v[i].w);
    }

    // 11. Get Child World Matrix
    for (int i = 0; i < 4; ++i)
    {
        const xxMatrix4& worldMatrix = child->WorldMatrix;
        step += snprintf(text + step, count - step, "Child Matrix (%u) : %f %f %f %f\n", i, worldMatrix.v[i].x, worldMatrix.v[i].y, worldMatrix.v[i].z, worldMatrix.v[i].w);
    }

    // 12. Get GrandChild World Matrix
    for (int i = 0; i < 4; ++i)
    {
        const xxMatrix4& worldMatrix = grandChild->WorldMatrix;
        step += snprintf(text + step, count - step, "GrandChild Matrix (%u) : %f %f %f %f\n", i, worldMatrix.v[i].x, worldMatrix.v[i].y, worldMatrix.v[i].z, worldMatrix.v[i].w);
    }

    // 13. xxMatrix Multiply
    xxMatrix4x4 a = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    xxMatrix4x4 b = { 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4 };
    xxMatrix4x4 c = a * b;
    for (int i = 0; i < 4; ++i)
    {
        step += snprintf(text + step, count - step, "Matrix Multiply (%u) : %f %f %f %f\n", i, c.v[i].x, c.v[i].y, c.v[i].z, c.v[i].w);
    }
    c.v[0].v = __builtin_vectormultiply(b.v[0].v, &a.v->v);
    c.v[1].v = __builtin_vectormultiply(b.v[1].v, &a.v->v);
    c.v[2].v = __builtin_vectormultiply(b.v[2].v, &a.v->v);
    c.v[3].v = __builtin_vectormultiply(b.v[3].v, &a.v->v);
    for (int i = 0; i < 4; ++i)
    {
        step += snprintf(text + step, count - step, "Matrix Multiply (%u) : %f %f %f %f\n", i, c.v[i].x, c.v[i].y, c.v[i].z, c.v[i].w);
    }
#if DirectXMath
    XMMATRIX aa = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    XMMATRIX bb = { 1, 0, 0, 0, 0, 2, 0, 0, 0, 0, 3, 0, 0, 0, 0, 4 };
    XMMATRIX cc = aa * bb;
    for (int i = 0; i < 4; ++i)
    {
        step += snprintf(text + step, count - step, "Matrix Multiply (%u) : %f %f %f %f\n", i, cc.r[i][0], cc.r[i][1], cc.r[i][2], cc.r[i][3]);
    }
#endif
}
//------------------------------------------------------------------------------
