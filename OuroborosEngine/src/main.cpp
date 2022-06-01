//Window opening contexts
#include <iostream>
#include <GLFW/glfw3.h>
#include <Graphics/Window.h>
#include <Graphics/shader.h>

#include "engine/engine.h"
#include <vulkan.h>

//Engine contexts
#include "engine/ecs_settings.h"
#include "engine/ecs/ecs_base.h"
#include "engine/ecs/components.h"
#include "engine/gui/gui_component_panel.h"


std::unique_ptr<Renderer::Window> window;
int main()
{
    window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Project"));

    OE::Engine::Get().Init();

    //For debug ECS manager
    auto& ent = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<Transform>(ent.myID, glm::vec3{0.f, 1.f, 0.f});
    ecs_manager.AddComponent<Velocity>(ent.myID, glm::vec3{ 2.f, 3.f, 0.f });
    auto& ent2 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<LifeTime>(ent2.myID, 5);
    ecs_manager.AddComponent<Mesh>(ent2.myID);
    auto& ent3 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<BoolWrapper>(ent3.myID, false);

    auto& ent4 = ecs_manager.CreateEntity();
    ecs_manager.AddComponent<Transform>(ent4.myID, glm::vec3{ 4.f, 5.f, 0.f });
    ecs_manager.AddComponent<Velocity>(ent4.myID, glm::vec3{ 6.f, 7.f, 0.f });
    ecs_manager.AddComponent<Tag>(ent4.myID, "TestEntity");
    std::cout << ecs_manager.MatchesSignature<Signature0>(ent.myID) << std::endl;
    std::cout << ecs_manager.MatchesSignature<Signature1>(ent.myID) << std::endl;

    ecs_manager.ForEntitiesMatching<Signature0>(1.2f,[](auto& ent, float dt, [[maybe_unused]]Transform& transform, [[maybe_unused]] Velocity& velocity)
    {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "Transform: " << transform.pos.x << ", " << transform.pos.y << std::endl;
            std::cerr << "Velocity : " << velocity.vel.x << ", " << velocity.vel.y << std::endl;
    });

    //Debug ECS manager ends here


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
    //window->vulkan_imgui_manager.RegisterPanel("Entities", [&](void)
    //    {
    //        manager.ForEntities([](OE::ecs_ID entID)
    //            {
    //                ImGui::Text("EntityID : %d", entID);
    //            });
    //    });

    std::cout << "Hello World!" << std::endl;

    while(!glfwWindowShouldClose(window->GetWindowData().window))
    {
        window->Update();
    }

    return 0;
}
