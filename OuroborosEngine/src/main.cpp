#include <GLFW/glfw3.h>
#include "engine/engine.h"

int main()
{
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