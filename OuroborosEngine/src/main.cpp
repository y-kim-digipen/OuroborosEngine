//Window opening contexts
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "engine/engine.h"
#include <vulkan.h>

#include "engine/ecs/ecs_base.h"
//#include "brigand/sequences/size.hpp"
#include "engine/ecs/component_manager.h"
#include "brigand/adapted.hpp"
#include "engine/ecs/components.h"

int SetupContextWindow()
{
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);

    glfwWindowHint(GLFW_SAMPLES, 1);                     // change for anti-aliasing
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glfwInit();

    const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Hello World!", NULL, NULL);

    if (window == nullptr) {
        std::cerr << "Failed to open GLFW window. Check if your GPU is compatible." << std::endl;
        glfwTerminate();
        return -1;
    }
    //Set up callback functions
    //glfwSetErrorCallback(&engine::GLFWErrorCallback);
    //glfwSetKeyCallback(m_pWindow, &engine::KeyboardInputCallback);

    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Failed to initialize GLEW." << std::endl;
        glfwTerminate();
        return -1;
    }
    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    return 0;
}

int main()
{
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

    while(true)
    {
        OE::Engine::Get().PreUpdate(0.0);
        OE::Engine::Get().Update(0.0);
        OE::Engine::Get().PostUpdate(0.0);
    }
    VkInstance instance;
    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    
    vkCreateInstance(&create_info, nullptr, &instance);

    OE::Engine::Get().CleanUp();
    OE::Engine::Get().ShutDown();
    return 0;
}