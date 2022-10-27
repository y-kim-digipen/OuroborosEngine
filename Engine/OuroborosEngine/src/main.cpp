#include <GLFW/glfw3.h>
#include "core/engine.h"
int main()
{
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
