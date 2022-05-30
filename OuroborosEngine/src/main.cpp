//Window opening contexts
#include <iostream>
#include <GLFW/glfw3.h>
#include <Graphics/Window.h>
#include <Graphics/shader.h>

#include "engine/engine.h"
#include <vulkan.h>

//Engine contexts
#include "engine/ecs/ecs_base.h"
#include "engine/ecs/components.h"


std::unique_ptr<Renderer::Window> window;
int main()
{
    //window = std::make_unique<Renderer::Window>(Renderer::WindowProperties("Project"));

    OE::Engine::Get().Init();

    using ComponentList = OE::ECS::TypeList<Transform, Velocity, LifeTime, Mesh, BoolWrapper>;

    using Signature0 = OE::ECS::types::Signature<Transform, Velocity>;
    using Signature1 = OE::ECS::types::Signature<LifeTime, Mesh>;
    using Signature2 = OE::ECS::types::Signature<BoolWrapper>;

    using SignatureList = OE::ECS::types::SignatureList<Signature0, Signature1, Signature2>;
    using OESettings = OE::ECS::_impl::Settings<ComponentList, SignatureList>;

    //std::cout << OESettings::CountComponent() << std::endl;
    //std::cout << OESettings::IsComponent<void*>() << std::endl;
    //std::cout << OESettings::SignatureIndex<Signature0>() << std::endl;

    using ECS_Manager = OE::ECS::Manager<OESettings>;
    ECS_Manager manager;
    auto& ent = manager.CreateEntity();
    manager.AddComponent<Transform>(ent.myID, 0.f, 1.f);
    manager.AddComponent<Velocity>(ent.myID, 2.f, 3.f);
    auto& ent2 = manager.CreateEntity();
    manager.AddComponent<LifeTime>(ent2.myID, 5);
    manager.AddComponent<Mesh>(ent2.myID);
    auto& ent3 = manager.CreateEntity();
    manager.AddComponent<BoolWrapper>(ent3.myID, false);

    auto& ent4 = manager.CreateEntity();
    manager.AddComponent<Transform>(ent4.myID, 4.f, 5.f);
    manager.AddComponent<Velocity>(ent4.myID, 6.f, 7.f);
    //std::cout << manager.MatchesSignature<Signature0>(ent.myID) << std::endl;
    //std::cout << manager.MatchesSignature<Signature1>(ent.myID) << std::endl;

    manager.ForEntitiesMatching<Signature0>(1.2f,[](auto& ent, float dt, [[maybe_unused]]Transform& transform, [[maybe_unused]] Velocity& velocity)
    {
            std::cerr << "Function call from entity : " << ent << " dt : " << dt << std::endl;
            std::cerr << "Transform: " << transform.x << ", " << transform.y << std::endl;
            std::cerr << "Velocity : " << velocity.x << ", " << velocity.y << std::endl;
    });


    std::cout << "Hello World!" << std::endl;

    while(!glfwWindowShouldClose(window->GetWindowData().window))
    {
        window->Update();
    }

    return 0;
}
