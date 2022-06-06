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
#include "Graphics/vulkan/vulkan_material.h"

static Renderer::Camera camera;
std::unique_ptr<Renderer::Window> window;

auto physics_system_impl = [](OE::ecs_ID id, float dt, TransformComponent& transform, VelocityComponent& velocity)
{
    transform.pos += velocity.vel;
};

void SetupGUI();

void ECS_TestSetup();

int main()
{
    window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Ouroboros Project"));

    OE::Engine::Get().Init();
    ECS_TestSetup();

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

    //Renderer::VulkanMaterial material();

    //dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->material_manager->AddMaterial("material", &material);

    ecs_manager.ForEntitiesMatching<PhysicsSystem>(0.0, physics_system_impl);

    SetupGUI();

    window->GetWindowData().RenderContextData.get()->InitGlobalData();

    while (!glfwWindowShouldClose(window->GetWindowData().window))
    {
        OE::Engine::Get().PreUpdate();
        OE::Engine::Get().Update();
        ecs_manager.UpdateSystem(OE::Engine::Get().delta_timer.GetDeltaTime());
        window->BeginFrame();
        window->Update();

        ecs_manager.ForEntitiesMatching<MeshDrawSignature>(0.f,
            [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] MeshComponent& mesh, [[maybe_unused]] ShaderComponent& shader) mutable
            {
                auto* context = dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get());
                if (ecs_manager.GetEntity(ent).alive)
                {
                    camera.data.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window->GetWidth()) / window->GetHeight(), 0.1f, 100.0f);
                    camera.data.view = camera.GetCameraMat();
                    //camera.data.projection[1][1] *= -1;
                    camera.data.view = glm::lookAt(camera.data.position, glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));
                    
                    //TODO: pass renderer camera data
                    context->global_data.position= camera.data.position;
                    context->global_data.view= camera.data.view;
                    context->global_data.projection = camera.data.projection;
                    context->UpdateGlobalData();
                    context->AddDrawQueue(&transform, nullptr , &mesh, &shader);
                }

            }
        );

        dynamic_cast<Renderer::VulkanContext*>(window->GetWindowData().RenderContextData.get())->DrawQueue();
        window->EndFrame();
        OE::Engine::Get().PostUpdate();
    }

    return 0;
}

void SetupGUI()
{
    window->vulkan_imgui_manager.RegisterMainMenu([]() mutable
        {
            if (ImGui::BeginMainMenuBar())
            {
                if (ImGui::BeginMenu("Editor"))
                {
                    auto& panels = window->vulkan_imgui_manager.GetPanels();
                    for (auto& [key, pair] : panels)
                    {
                        bool& open = pair.second;
                        if (ImGui::MenuItem(key.c_str(), nullptr, open))
                        {
                            open = !open;
                        }
                    }
                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }
        });

    window->vulkan_imgui_manager.RegisterPanel("EngineInfo", &OE::EngineInfoPanelFunction);
    window->vulkan_imgui_manager.RegisterPanel("Entities", &OE::EntityInfoPanelFunction);
    window->vulkan_imgui_manager.RegisterPanel("SystemInfo", &OE::SystemInfoPanelFunction);
}

void ECS_TestSetup()
{
    auto& ent = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<TransformComponent>(ent.myID, glm::vec3{ 0.f, 1.f, 0.f });
    ecs_manager.AddComponent<VelocityComponent>(ent.myID, glm::vec3{ 2.f, 3.f, 0.f });
    auto& ent2 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<LifeTimeComponent>(ent2.myID, 5);
    ecs_manager.AddComponent<MeshComponent>(ent2.myID);
    auto& ent3 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<BoolWrapperComponent>(ent3.myID, false);

    auto& ent4 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<TransformComponent>(ent4.myID, glm::vec3{ 4.f, 5.f, 0.f });
    ecs_manager.AddComponent<VelocityComponent>(ent4.myID, glm::vec3{ 6.f, 7.f, 0.f });
    ecs_manager.AddComponent<TagComponent>(ent4.myID, "TestEntity");

    ecs_manager.ForEntitiesMatching<PhysicsSystem>(1.2f, [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
        {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
            std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
        });

    ecs_manager.ForEntitiesMatching<Signature0>(1.2f, [](auto& ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
        {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
            std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
        });

    ecs_manager.system_storage.RegisterSystemImpl<PhysicsSystem>([](OE::ecs_ID ent, float dt, [[maybe_unused]] TransformComponent& transform, [[maybe_unused]] VelocityComponent& velocity)
        {
            transform.pos += velocity.vel * dt;
        });
}