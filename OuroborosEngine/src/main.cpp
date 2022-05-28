//Window opening contexts
#include <iostream>
#include <GLFW/glfw3.h>
#include <Graphics/Window.h>
#include <Graphics/shader.h>


std::unique_ptr<Renderer::Window> window;
int main()
{
    window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Project"));

    std::cout << "Hello World!" << std::endl;

    while(!glfwWindowShouldClose(window->GetWindowData().window))
    {
        window->Update();
    }

    return 0;
}
