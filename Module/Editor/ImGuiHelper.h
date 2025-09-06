//==============================================================================
// Minamoto : ImGui Helper
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>

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
    struct Chain
    {
        std::string*            Str;
        ImGuiInputTextCallback  ChainCallback;
        void*                   ChainCallbackUserData;
        static int Callback(ImGuiInputTextCallbackData* data)
        {
            Chain* chain = (Chain*)data->UserData;
            if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
            {
                chain->Str->resize(data->BufTextLen);
                data->Buf = chain->Str->data();
            }
            else if (chain->ChainCallback)
            {
                data->UserData = chain->ChainCallbackUserData;
                return chain->ChainCallback(data);
            }
            return 0;
        }
    };
    Chain chain = { &text, callback, user_data };
    return InputTextEx(label, hint, text.data(), (int)text.capacity() + 1, size, flags | ImGuiInputTextFlags_CallbackResize, Chain::Callback, &chain);
}
//---------------------------------------------------------------------------
inline bool InputTextMultiline(const char* label, std::string& text, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL)
{
    return InputTextEx(label, nullptr, text, size, flags | ImGuiInputTextFlags_Multiline, callback, user_data);
}
//---------------------------------------------------------------------------
inline bool ListBox(const char* label, int* current_item, int* focus_item, const char* (*getter)(void* user_data, int idx), void* user_data, int items_count, int height_in_items = -1)
{
    ImGuiContext& g = *ImGui::GetCurrentContext();

    // Calculate size from "height_in_items"
    if (height_in_items < 0)
        height_in_items = ImMin(items_count, 7);
    float height_in_items_f = height_in_items + 0.25f;
    ImVec2 size(0.0f, ImTrunc(GetTextLineHeightWithSpacing() * height_in_items_f + g.Style.FramePadding.y * 2.0f));

    if (!BeginListBox(label, size))
        return false;

    // Assume all items have even height (= 1 line of text). If you need items of different height,
    // you can create a custom version of ListBox() in your code without using the clipper.
    bool value_changed = false;
    ImGuiListClipper clipper;
    clipper.Begin(items_count, GetTextLineHeightWithSpacing()); // We know exactly our line height here so we pass it as a minor optimization, but generally you don't need to.
    clipper.IncludeItemByIndex(*current_item);
    clipper.IncludeItemByIndex(*focus_item);
    while (clipper.Step())
    {
        for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
        {
            const char* item_text = getter(user_data, i);
            if (item_text == NULL)
                item_text = "*Unknown item*";
            
            PushID(i);
            const bool item_selected = (i == *current_item);
            if (Selectable(item_text, item_selected))
            {
                *current_item = i;
                value_changed = true;
            }
            if (item_selected)
                SetItemDefaultFocus();
            if (i == (*focus_item))
                ScrollToItem();
            PopID();
        }
    }
    EndListBox();

    if (value_changed)
        MarkItemEdited(g.LastItemData.ID);

    return value_changed;
}
//---------------------------------------------------------------------------
} // namespace ImGui
//---------------------------------------------------------------------------
