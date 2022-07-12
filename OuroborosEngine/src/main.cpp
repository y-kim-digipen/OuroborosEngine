#include <GLFW/glfw3.h>
#include "engine/engine.h"
#include "engine/scripting/lua_script_manager.h"
#include <yaml-cpp/yaml.h>

int main()
{
    YAML::Emitter emitter;
    OE::Engine::Init();
    while (!glfwWindowShouldClose(OE::Engine::GetGLFWWindow()))
    {
        OE::Engine::PreUpdate();

        OE::Engine::Update();

        OE::Engine::PostUpdate();
    }
    OE::Engine::ShutDown();
    return 0;
}
