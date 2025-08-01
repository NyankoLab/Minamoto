//==============================================================================
// Minamoto : Dear ImGui Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include <sys/stat.h>
#if defined(__ANDROID__)
#   include <imgui/imgui.h>
#elif defined(__APPLE__)
#   include <TargetConditionals.h>
#   include <CoreGraphics/CoreGraphics.h>
#if TARGET_OS_OSX
#   include <Cocoa/Cocoa.h>
#elif TARGET_OS_IOS
#   include <UIKit/UIKit.h>
#endif
#   include <imgui/backends/imgui_impl_osx.h>
#elif defined(_WIN32)
#   define WIN32_LEAN_AND_MEAN
#   include <windows.h>
#   include <imgui/backends/imgui_impl_win32.h>
#endif
#include <imgui/misc/freetype/imgui_freetype.h>
#include <Runtime/Runtime.h>
#include <IconFontCppHeaders/IconsFontAwesome4.h>
#include "implement/imgui_impl_xx.h"
#include "Module.h"
#include "Renderer.h"
#include "DearImGui.h"

// Allocator Wrapper
static void* MallocWrapper(size_t size, void* user_data)    { IM_UNUSED(user_data); return calloc(size, 1); }
static void  FreeWrapper(void* ptr, void* user_data)        { IM_UNUSED(user_data); free(ptr);              }

char const* DearImGui::g_graphicShortName = nullptr;
bool        DearImGui::g_recreateWindow = false;
#if defined(xxIOS)
bool        DearImGui::g_powerSaving = false;
#else
bool        DearImGui::g_powerSaving = true;
#endif
bool        DearImGui::g_demoWindow = false;
//==============================================================================
//  Dear ImGui
//==============================================================================
void DearImGui::Create(void* view, float scale, float font)
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::SetAllocatorFunctions(MallocWrapper, FreeWrapper);
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;
    io.IniFilename = nullptr;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(scale);             // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleMain = 1.0f / font;
    style.FontScaleDpi = scale;             // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)
    io.ConfigDpiScaleFonts = true;          // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    io.ConfigDpiScaleViewports = true;      // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Read 'misc/fonts/README.txt' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != NULL);

    // Load / Merge Fonts
    ImFontConfig font_config;
    font_config.SizePixels          = 13.0f * font;
    font_config.RasterizerMultiply  = 1.0f;
    io.Fonts->AddFontDefault(&font_config);

    static ImWchar const icons_ranges[] = { ICON_MIN_FA, ICON_MAX_16_FA, 0 };
    font_config.MergeMode           = true;
    font_config.PixelSnapH          = true;
    font_config.GlyphOffset.y       = 2.0f * font;
    font_config.GlyphMinAdvanceX    = 13.0f * font * 2.0f;
#if defined(xxMACOS)
    io.Fonts->AddFontFromFileTTF("../Resources/" FONT_ICON_FILE_NAME_FA, 13.0f * font, &font_config, icons_ranges);
#elif defined(xxWINDOWS)
    io.Fonts->AddFontFromFileTTF("module/" FONT_ICON_FILE_NAME_FA, 13.0f * font, &font_config, icons_ranges);
#endif
    font_config.GlyphOffset.y       = 0.0f;
    font_config.GlyphMinAdvanceX    = 0.0f;

    static ImWchar const fonts_ranges[] = { 0x8000, 0xFFFF, 0 };
    font_config.OversampleH         = 1;
    font_config.OversampleV         = 1;
    font_config.PixelSnapH          = true;
    font_config.MergeMode           = true;
    font_config.SizePixels          = 13.0f * font;
    font_config.RasterizerMultiply  = 2.0f / font;
#if defined(xxMACOS)
    io.Fonts->AddFontFromFileTTF("/System/Library/Fonts/PingFang.ttc", 13.0f, &font_config, fonts_ranges);
#elif defined(xxWINDOWS)
    if (GetFileAttributesA("C:\\Windows\\Fonts\\meiryo.ttc") != INVALID_FILE_ATTRIBUTES)
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\meiryo.ttc", 13.0f * font, &font_config, fonts_ranges);
    else if (GetFileAttributesA("C:\\Windows\\Fonts\\msjh.ttc") != INVALID_FILE_ATTRIBUTES)
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\msjh.ttc", 13.0f * font, &font_config, fonts_ranges);
#endif

    // Setup Platform/Renderer bindings
#if defined(xxMACOS)
    NSView* nsView = (__bridge NSView*)view;
    ImGui_ImplOSX_Init(nsView);
#elif defined(xxWINDOWS)
    ImGui_ImplWin32_Init(view);
#endif
    ImGui_ImplXX_Init(Renderer::g_instance, Renderer::g_device, Renderer::g_renderPass);
    ImGui_ImplXX_NewFrame();
}
//------------------------------------------------------------------------------
void DearImGui::Shutdown()
{
    ImGui_ImplXX_Shutdown();
#if defined(xxMACOS)
    ImGui_ImplOSX_Shutdown();
#elif defined(xxWINDOWS)
    ImGui_ImplWin32_Shutdown();
#endif
    ImGui::DestroyContext();
}
//------------------------------------------------------------------------------
void DearImGui::Suspend()
{
    ImGui_ImplXX_InvalidateDeviceObjects();
}
//------------------------------------------------------------------------------
void DearImGui::Resume()
{
    ImGui_ImplXX_CreateDeviceObjects();
}
//------------------------------------------------------------------------------
void DearImGui::NewFrame(void* view)
{
    static float previousTime;
    float time = xxGetCurrentTime();
    ImGui::GetIO().DeltaTime = time - previousTime;
    previousTime = time;

    // Start the Dear ImGui frame
    ImGui_ImplXX_NewFrame();
#if defined(xxMACOS)
    ImGui_ImplOSX_NewFrame((__bridge NSView*)view);
#elif defined(xxIOS)
    CGFloat contentScaleFactor = ((CGFloat(*)(id, SEL, ...))objc_msgSend)((__bridge id)view, sel_registerName("contentScaleFactor"));
#if TARGET_CPU_X86_64
    CGRect rect = ((CGRect(*)(id, SEL, ...))objc_msgSend_stret)((__bridge id)view, sel_registerName("bounds"));
#else
    CGRect rect = ((CGRect(*)(id, SEL, ...))objc_msgSend)((__bridge id)view, sel_registerName("bounds"));
#endif
    ImGui::GetIO().DisplaySize = ImVec2(rect.size.width, rect.size.height);
    ImGui::GetIO().DisplayFramebufferScale = ImVec2(contentScaleFactor, contentScaleFactor);
#elif defined(xxWINDOWS)
    ImGui_ImplWin32_NewFrame();
#else
    ImGui::GetIO().DisplaySize = ImVec2(Renderer::g_width, Renderer::g_height);
#endif
    ImGui::NewFrame();

    static bool showAbout = false;

    // Graphic API
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("Minamoto"))
        {
            ImGui::MenuItem("About Minamoto", nullptr, &showAbout);
            ImGui::Separator();
            ImGui::MenuItem("Power Saving", nullptr, &g_powerSaving);
            ImGui::Separator();

            char const* deviceStringCurrent = Renderer::GetCurrentFullName();
            char const* deviceStringTarget = "";

            for (int i = 0; i < 32; ++i)
            {
                deviceStringTarget = Renderer::GetGraphicFullName(i);
                if (deviceStringTarget == nullptr)
                    break;
                bool selected = (strcmp(deviceStringTarget, deviceStringCurrent) == 0);
                if (ImGui::MenuItem(deviceStringTarget, nullptr, &selected))
                {
                    g_graphicShortName = Renderer::GetGraphicShortName(i);
                    break;
                }
            }

#if defined(xxWINDOWS)
            // We need to create window when device is running in FlipEx Mode
            if (g_graphicShortName != nullptr)
            {
                bool flipCurrent =  strstr(deviceStringCurrent, "Ex") || \
                                    strstr(deviceStringCurrent, "10.") || \
                                    strstr(deviceStringCurrent, "11.") || \
                                    strstr(deviceStringCurrent, "12.");
                bool flipTarget =   strstr(deviceStringTarget, "Ex") || \
                                    strstr(deviceStringTarget, "10.") || \
                                    strstr(deviceStringTarget, "11.") || \
                                    strstr(deviceStringTarget, "12.");
                g_recreateWindow = (flipCurrent && flipTarget == false);
            }
#endif

            ImGui::Separator();
            ImGui::MenuItem("Demo Window", nullptr, &g_demoWindow);
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (showAbout)
    {
        if (ImGui::Begin("About Minamoto", &showAbout, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
        {
            ImGui::Text("%s Based on xxGraphic", Runtime::Version);
            ImGui::TextUnformatted("Build : " __DATE__ " " __TIME__);
            ImGui::Separator();
            ImGui::DumpBuildInformation();
            ImGui::End();
        }
    }
}
//------------------------------------------------------------------------------
bool DearImGui::Update(bool demo)
{
    if (g_powerSaving == false && ImGui::BeginMainMenuBar())
    {
        char text[64];
        snprintf(text, sizeof(text), "%.1f FPS ", ImGui::GetIO().Framerate);
        ImGui::SetCursorPosX(ImGui::GetIO().DisplaySize.x - ImGui::CalcTextSize(text).x);
        ImGui::TextUnformatted(text);
        if (ImGui::IsItemHovered())
        {
            time_t t = time(NULL);
#if defined(xxWINDOWS)
            struct tm tm;
            localtime_s(&tm, &t);
            strftime(text, sizeof(text), "%c", &tm);
#else
            struct tm* tm = localtime(&t);
            strftime(text, sizeof(text), "%c", tm);
#endif
            ImGui::SetTooltip("%s", text);
        }
        ImGui::EndMainMenuBar();
    }

    if (demo == false)
    {
#ifndef IMGUI_DISABLE_DEMO_WINDOWS
        if (g_demoWindow)
        {
            ImGui::ShowDemoWindow(&g_demoWindow);
        }
#endif
        ImGui::EndFrame();
        ImGui::Render();
        return g_powerSaving == false;
    }
#ifndef IMGUI_DISABLE_DEMO_WINDOWS
    // Global data for the demo
    static bool show_demo_window = true;
    static bool show_another_window = false;

    // 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
    if (show_demo_window)
        ImGui::ShowDemoWindow(&show_demo_window);

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
    {
        static float f = 0.0f;
        static int counter = 0;

        ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

        ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
        ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
        ImGui::ColorEdit3("clear color", Renderer::g_clearColor); // Edit 3 floats representing a color

        if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::Text("Time : %.3f", xxGetCurrentTime());
        ImGui::End();
    }

    // 3. Show another simple window.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }
#endif
    // Rendering
    ImGui::EndFrame();
    ImGui::Render();

    return true;
}
//------------------------------------------------------------------------------
void* DearImGui::PostUpdate(void* view, bool render)
{
    // Update and Render additional Platform Windows
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        if (render)
        {
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    // Recreate Graphic API
    if (g_graphicShortName != nullptr)
    {
        char const* graphicShortName = g_graphicShortName;
        g_graphicShortName = nullptr;

        Module::Message({ "SHUTDOWN" });
        ImGui_ImplXX_Shutdown();
#if defined(xxMACOS)
        ImGui_ImplOSX_Shutdown();
#elif defined(xxWINDOWS)
        ImGui_ImplWin32_Shutdown();
#endif
        Renderer::Shutdown();

        // To recreate window that we need to reset window setting
        if (g_recreateWindow)
        {
            g_recreateWindow = false;

#if defined(xxWINDOWS)
            wchar_t className[256] = {};
            wchar_t textName[256] = {};
            DWORD style = 0;
            RECT rect = { 0, 0, 1280, 720 };
            HWND hWndParent = nullptr;
            HMODULE hInstance = nullptr;
            GetClassNameW((HWND)view, className, 256);
            GetWindowTextW((HWND)view, textName, 256);
            GetWindowRect((HWND)view, &rect);
            style = (DWORD)GetWindowLongW((HWND)view, GWL_STYLE);
            hWndParent = (HWND)GetWindowLongPtrW((HWND)view, GWLP_HWNDPARENT);
            hInstance = (HMODULE)GetWindowLongPtrW((HWND)view, GWLP_HINSTANCE);
            ::DestroyWindow((HWND)view);
            view = ::CreateWindowW(className, textName, style, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, hWndParent, nullptr, hInstance, nullptr);
            ::ShowWindow((HWND)view, SW_SHOWDEFAULT);
            ::UpdateWindow((HWND)view);
#endif
        }

#if defined(xxMACOS) || defined(xxIOS)
        id window = objc_msgSend((__bridge id)view, sel_getUid("window"));
        Renderer::Create((__bridge void*)window, Renderer::g_width, Renderer::g_height, graphicShortName);
#else
        Renderer::Create(view, Renderer::g_width, Renderer::g_height, graphicShortName);
#endif
#if defined(xxMACOS)
        NSView* nsView = (__bridge NSView*)view;
        ImGui_ImplOSX_Init(nsView);
#elif defined(xxWINDOWS)
        ImGui_ImplWin32_Init(view);
#endif
        ImGui_ImplXX_Init(Renderer::g_instance, Renderer::g_device, Renderer::g_renderPass);
        ImGui_ImplXX_NewFrame();
        Module::Message({ "INIT" });
    }

    return view;
}
//------------------------------------------------------------------------------
void DearImGui::Render(uint64_t commandEncoder)
{
    ImDrawData* drawData = ImGui::GetDrawData();
    drawData->FramebufferScale = ImVec2(Renderer::g_scale, Renderer::g_scale);
    ImGui_ImplXX_RenderDrawData(drawData, commandEncoder);
}
//------------------------------------------------------------------------------
bool DearImGui::PowerSaving()
{
    return g_powerSaving;
}
//------------------------------------------------------------------------------
#if defined(xxANDROID)
void DearImGui::HandleEventAndroid(int type, float x, float y)
{
    ImGuiIO& io = ImGui::GetIO();
    switch (type)
    {
    case 0: // ACTION_DOWN
        io.MousePos = ImVec2(x, y);
        io.MouseDown[0] = true;
        break;
    case 1: // ACTION_UP
        io.MousePos = ImVec2(x, y);
        io.MouseDown[0] = false;
        break;
    case 2: // ACTION_MOVE
        io.MousePos = ImVec2(x, y);
        break;
    case 3: // ACTION_CANCEL
    case 4: // ACTION_OUTSIDE
    case 7: // ACTION_HOVER_MOVE
    case 9: // ACTION_HOVER_ENTER
    case 10:// ACTION_HOVER_EXIT
        io.MouseDown[0] = false;
        break;
    case 5: // ACTION_POINTER_DOWN
    case 6: // ACTION_POINTER_UP
    case 8: // ACTION_SCROLL
        break;
    }
}
#endif
//------------------------------------------------------------------------------
