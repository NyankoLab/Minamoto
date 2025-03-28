//==============================================================================
// Minamoto : LuaConsole Source
//
// Copyright (c) 2023-2025 TAiGA
// https://github.com/metarutaiga/minamoto
//==============================================================================
#include "Editor.h"
#include <queue>
#include <string>
#include "Script/Lua.h"
#include "Component/Console.h"
#include "LuaConsole.h"

extern "C"
{
#define LUA_LIB
#define LUAEX_API
#define LUA_USER_H "../../Build/include/luauser.h"
#include <lua/lua.h>
#include <lua/lauxlib.c>
#include <lua/liolib.c>
#include <lua/loadlib.c>
#include <lua/loslib.c>
#define luaL_openlibs(...)
#define main lua_main
#include <lua/lua.c>
#undef main
#undef lua_initreadline
#undef lua_readline
#undef lua_saveline
#undef lua_freeline
#undef lua_writeline
#undef lua_writestring
#undef lua_writestringerror
static int  lua_readline(lua_State* L, char* buffer, char const* prompt);
static void lua_saveline(lua_State* L, char const* line);
static void lua_writeline();
static void lua_writestring(char const* string, size_t length);
static void lua_writestringerror(char const* string, char const* parameter);
}

static Console console;
static std::deque<std::string> Outputs;
//------------------------------------------------------------------------------
void LuaConsole::Initialize()
{
    Outputs.push_back(std::string());

    lua_pinitreadline = [](lua_State*){};
    lua_preadline = lua_readline;
    lua_psaveline = lua_saveline;
    lua_pfreeline = [](lua_State*, char*){};
    lua_pwriteline = lua_writeline;
    lua_pwritestring = lua_writestring;
    lua_pwritestringerror = lua_writestringerror;

    static luaL_Reg const loadedlibs[] =
    {
        { LUA_LOADLIBNAME, luaopen_package },
        { LUA_IOLIBNAME, luaopen_io },
        { LUA_OSLIBNAME, luaopen_os },
        { NULL, NULL }
    };

    lua_gc(Lua::L, LUA_GCSTOP);  /* stop GC while building state */
    for (luaL_Reg const* lib = loadedlibs; lib->func; lib++)
    {
        luaL_requiref(Lua::L, lib->name, lib->func, 1);
        lua_pop(Lua::L, 1);  /* remove lib */
    }
    lua_gc(Lua::L, LUA_GCRESTART);  /* start GC... */
    lua_gc(Lua::L, LUA_GCGEN, 0, 0);  /* ...in generational mode */

    print_version();
}
//------------------------------------------------------------------------------
void LuaConsole::Shutdown()
{
    Outputs = std::deque<std::string>();
}
//------------------------------------------------------------------------------
bool LuaConsole::Update(const UpdateData& updateData, bool& show)
{
    if (show == false)
        return false;

    bool update = false;
    if (ImGui::Begin(ICON_FA_LAPTOP "Lua Console", &show))
    {
        if (ImGui::IsWindowFocused())
        {
            update |= console.UpdateInput(updateData);
        }

        bool appearing = ImGui::IsWindowAppearing();
        if (appearing || ImGui::IsKeyReleased(ImGuiKey_Enter))
        {
            auto& input = console.input;

            char c = 0;
            if (appearing && input.empty() == false)
                std::swap(input[0], c);

            auto L = Lua::L;
            for (size_t i = 0; i < 2; ++i)
            {
                int status;
                if ((status = loadline(L)) != -1)
                {
                    if (status == LUA_OK)
                        status = docall(L, 0, LUA_MULTRET);
                    if (status == LUA_OK)
                        l_print(L);
                    else
                        report(L, status);
                }
            }
            lua_settop(L, 0);  /* clear stack */
            update = true;

            if (appearing && input.empty() == false)
                std::swap(input[0], c);
        }

        auto const& lines = Outputs;
        update |= console.UpdateConsole(updateData, lines);
    }
    ImGui::End();

    return update;
}
//==============================================================================
//  Standard I/O
//==============================================================================
static int lua_readline(lua_State* L, char* buffer, char const* prompt)
{
    auto& line = Outputs.back();
    if (line != prompt)
    {
        for (char c; (c = *prompt); ++prompt)
        {
            if (c == '\b' || c == 0x7F)
            {
                if (line.empty() == false)
                    line.pop_back();
                continue;
            }
            line.append(1, c);
        }
    }
    auto& input = console.input;
    auto& inputPos = console.inputPos;
    if (input.empty() || input[0] == 0)
        return 0;
    lua_writestring(input.c_str(), input.size());
    lua_writeline();
    strncpy(buffer, input.c_str(), LUA_MAXINPUT);
    size_t erase = std::min<size_t>(input.size(), LUA_MAXINPUT);
    input.erase(input.begin(), input.begin() + erase);
    inputPos -= inputPos;
    return 1;
}
//------------------------------------------------------------------------------
static void lua_saveline(lua_State* L, char const* line)
{
    console.AddHistory(line);
}
//------------------------------------------------------------------------------
static void lua_writeline()
{
    Outputs.push_back(std::string());
}
//------------------------------------------------------------------------------
static void lua_writestring(char const* string, size_t length)
{
    for (size_t i = 0; i < length; ++i)
    {
        auto& line = Outputs.back();
        char c = string[i];
        switch (c)
        {
        case 0:
            return;
        case '\b':
        case 0x7F:
            if (line.empty() == false)
                line.pop_back();
            continue;
        case '\n':
                Outputs.push_back(std::string());
            continue;
        case '\r':
            if (i + 1 < length && string[i + 1] != '\n')
                line.clear();
            continue;
        case '\t':
            line.resize((line.size() + 3) & ~3, ' ');
            continue;
        default:
            line.append(1, c);
            break;
        }
    }
}
//------------------------------------------------------------------------------
static void lua_writestringerror(char const* string, char const* parameter)
{
    size_t count = snprintf(nullptr, 0, string, parameter);
    char* temp = xxAlloc(char, count + 1);
    snprintf(temp, count + 1, string, parameter);
    lua_writestring(temp, count);
    xxFree(temp);
}
//==============================================================================
