//Window opening contexts
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vulkan.h>

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
    SetupContextWindow();

    VkInstance instance;
    VkInstanceCreateInfo create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
    
    vkCreateInstance(&create_info, nullptr, &instance);

    std::cout << "Hello World!" << std::endl;
    return 0;
}