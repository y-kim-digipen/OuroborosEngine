#include <GLFW/glfw3.h>
#include "engine/engine.h"

extern "C"
{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

#ifdef _WIN32
#pragma comment(lib, "external/lib/lua-5.4.2/liblua54.a")
#endif

int main()
{
    std::string cmd = "a = 7 + 11";
    lua_State* L = luaL_newstate();

    int r = luaL_dostring(L, cmd.c_str());

    if(r == LUA_OK)
    {
        lua_getglobal(L, "a");
        if(lua_isnumber(L, -1))
        {
            float a_in_cpp = (float)(lua_tonumber(L, -1));
            std::cout << a_in_cpp;
        }
    }
    else
    {
        std::string errormsg = lua_tostring(L, -1);
    }

    lua_close(L);


    OE::Engine::Init();
    while (!glfwWindowShouldClose(OE::Engine::GetRenderWindow()->GetWindowData().window))
    {
        OE::Engine::PreUpdate();
        OE::Engine::Update();
        OE::Engine::PostUpdate();
    }
    OE::Engine::ShutDown();
    return 0;
}