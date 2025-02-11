//==============================================================================
// Minamoto : ImGui Helper
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include <imgui/imgui.h>

//---------------------------------------------------------------------------
namespace ImGui
{
//---------------------------------------------------------------------------
inline bool InputChar(const char* label, char* v, char step, char step_fast, ImGuiInputTextFlags flags)
{
    return InputScalar(label, ImGuiDataType_S8, (void*)v, (void*)(step > 0 ? &step : NULL), (void*)(step_fast > 0 ? &step_fast : NULL), "%d", flags);
}
//---------------------------------------------------------------------------
inline bool SliderChar(const char* label, char* v, char v_min, char v_max, const char* format = nullptr, ImGuiSliderFlags flags = 0)
{
    return SliderScalar(label, ImGuiDataType_S8, v, &v_min, &v_max, format, flags);
}
//---------------------------------------------------------------------------
inline bool InputTextEx(const char* label, const char* hint, std::string& text, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
{
    int length = (int)text.length();
    text.resize(length + 1024);
    if (InputTextEx(label, hint, text.data(), length + 1024, size, flags, callback, user_data))
    {
        text.resize(strlen(text.c_str()));
        return true;
    }
    text.resize(length);
    return false;
}
//---------------------------------------------------------------------------
inline bool InputTextMultiline(const char* label, std::string& text, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
{
    return InputTextEx(label, nullptr, text, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
}
//---------------------------------------------------------------------------
} // namespace ImGui
//---------------------------------------------------------------------------
