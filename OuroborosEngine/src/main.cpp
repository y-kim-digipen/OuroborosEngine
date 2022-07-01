#include <GLFW/glfw3.h>
#include "engine/engine.h"
#include "engine/scripting/lua_script_manager.h"


int main()
{
    OE::Engine::Init();
    OE::Script::TestLua();
    while (!glfwWindowShouldClose(OE::Engine::GetRenderWindow()->GetWindowData().window))
    {
        OE::Engine::PreUpdate();

        OE::Engine::Update();

        OE::Engine::PostUpdate();
    }
    OE::Engine::ShutDown();
    return 0;
}
