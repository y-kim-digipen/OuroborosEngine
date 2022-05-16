//Window opening contexts
#include <iostream>
#include <GLFW/glfw3.h>
#include <Graphics/Window.h>
#include <Graphics/shader.h>
#include <Graphics/vulkan/vulkan_shader.h>


std::unique_ptr<Renderer::Window> window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Project"));

int main()
{

    std::cout << "Hello World!" << std::endl;

    std::unique_ptr<Renderer::Shader> shader = std::make_unique<Renderer::VulkanShader>();

    Renderer::ShaderConfig shader_config{
        "shader",
        {
            Renderer::E_StageType::VERTEX_SHADER,
            Renderer::E_StageType::FRAGMENT_SHADER
        },
        2
    };

    shader->Init(&shader_config);

    while(!glfwWindowShouldClose(window->GetWindowData().window))
    {
        window->Update();
    }
    /*SetupContextWindow();

    VkInstance instance;
    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    
    vkCreateInstance(&create_info, nullptr, &instance);*/

    return 0;
}
