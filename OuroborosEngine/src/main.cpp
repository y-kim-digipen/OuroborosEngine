//Window opening contexts
#include <iostream>
#include <GLFW/glfw3.h>
#include <Graphics/Window.h>
#include <Graphics/shader.h>

#include "engine/engine.h"
#include <vulkan.h>

//Engine contexts
#include <gtc/matrix_transform.hpp>

#include "engine/ecs_settings.h"
#include "engine/ecs/ecs_base.h"
#include "engine/ecs/components.h"
#include "engine/gui/gui_component_panel.h"
#include "Graphics/shader_manager.h"
#include "Graphics/vulkan/vulkan_context.h"
#include "Graphics/camera.h"

static Renderer::Camera camera;

auto physics_system_impl = [](OE::ecs_ID id, float ft, TransformComponent& transform, Velocity& velocity)
{
    transform.pos += velocity.vel;
};

std::unique_ptr<Renderer::Window> window;
int main()
{
    window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));

    OE::Engine::Get().Init();

    //For debug ECS manager
    auto& ent = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<TransformComponent>(ent.myID, glm::vec3{0.f, 1.f, 0.f});
    ecs_manager.AddComponent<Velocity>(ent.myID, glm::vec3{ 2.f, 3.f, 0.f });
    auto& ent2 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<LifeTime>(ent2.myID, 5);
    ecs_manager.AddComponent<MeshComponent>(ent2.myID);
    auto& ent3 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<BoolWrapper>(ent3.myID, false);

    auto& ent4 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<TransformComponent>(ent4.myID, glm::vec3{ 4.f, 5.f, 0.f });
    ecs_manager.AddComponent<Velocity>(ent4.myID, glm::vec3{ 6.f, 7.f, 0.f });
    ecs_manager.AddComponent<Tag>(ent4.myID, "TestEntity");

    

    std::cout << ecs_manager.MatchesSignature<Signature0>(ent.myID) << std::endl;
    std::cout << ecs_manager.MatchesSignature<Signature1>(ent.myID) << std::endl;

    ecs_manager.ForEntitiesMatching<PhysicsSystem>(1.2f,[](auto& ent, float dt, [[maybe_unused]]TransformComponent& transform, [[maybe_unused]] Velocity& velocity)
    {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "TransformComponent: " << transform.pos.x << ", " << transform.pos.y << std::endl;
            std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
    });

    ecs_manager.ForEntitiesMatching<Signature0>(1.2f, [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] Velocity& velocity)
        {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "TransformComponent: " << transform.pos.x << ", " << transform.pos.y << std::endl;
            std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
        });

	camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
    camera.data.view = camera.GetCameraMat();


    //Debug ECS manager ends here
    Renderer::ShaderConfig shader_config{
    "shader",
    {
        Renderer::E_StageType::VERTEX_SHADER,
        Renderer::E_StageType::FRAGMENT_SHADER
    },
    2
    };


    dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->shader_manager_.AddShader(&shader_config);

    ecs_manager.ForEntitiesMatching<PhysicsSystem>(1.2f, physics_system_impl);

    window->vulkan_imgui_manager.RegisterMainMenu([]()
        {
            if (ImGui::BeginMainMenuBar())
            {
                if(ImGui::BeginMenu("Editor"))
                {
                    auto& panels = window->vulkan_imgui_manager.GetPanels();
                    for (auto [key, pair] : panels)
                    {
                        bool& open = pair.second;
                        if(ImGui::MenuItem(key.c_str()))
                        {
                            std::cout << std::boolalpha << open << std::endl;
                            open = !open;
                            //ImGui::EndMenu();
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        });

    window->vulkan_imgui_manager.RegisterPanel("Entities", &OE::EntityInfoPanelFunction);
    //    });

    std::cout << "Hello World!" << std::endl;

    while(!glfwWindowShouldClose(window->GetWindowData().window))
    {
        window->BeginFrame();
        window->Update();


    	ecs_manager.ForEntitiesMatching<MeshDrawSignature>(0.f,
            [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] MeshComponent& mesh, [[maybe_unused]] ShaderComponent& shader) mutable
        {
            auto* context = dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get());
            static bool init = true;
            if (init)
            {
                context->mesh_manager_.AddMesh("suzanne");
                init = false;
            }
            else
            {
                if (ecs_manager.GetEntity(ent).alive)
                {
                    camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
                    camera.data.view = camera.GetCameraMat();
                    context->shader_manager_.GetShader("shader")->SetUniformValue("projection", (void*)&camera.data.projection);
                    context->shader_manager_.GetShader("shader")->SetUniformValue("view", (void*)&camera.data.view);

                    context->AddDrawQueue(&transform, nullptr, &mesh, nullptr);
                }
            }
        });

        dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->DrawQueue();
        window->EndFrame();
    }

    return 0;
}
