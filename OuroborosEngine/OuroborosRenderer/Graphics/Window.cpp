#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>
#include "Window.h"
#include "iostream"
#include "vulkan/vulkan_context.h"
#include "opengl/opengl_context.h"

#include "shader.h"

namespace Renderer
{

	Window::Window(const WindowProperties& window_properties)
	{
		// make unique pointer
		Init(window_properties);
	
	}

	Window::~Window()
	{
		Shutdown();
	}

	void Window::Update()
	{
		glfwPollEvents();
		
		window_data.RenderContextData->BeginFrame();
		//vulkan_imgui_manager.Update();
		window_data.RenderContextData->DrawMesh("shader", "cube");
		//vulkan_imgui_manager.EndFrame();
		window_data.RenderContextData->EndFrame();
		//TODO : make close
		if (glfwWindowShouldClose(window_data.window))
		{
			
		}
		if (glfwGetKey(window_data.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
		
		}
	}

	void Window::Init(const WindowProperties& window_properties)
	{
		window_data.height = window_properties.Height;
		window_data.width = window_properties.Width;
		window_data.title = window_properties.title;

		if (!GLFW_IsInit)
		{
			int IsSuccess = glfwInit();
			//set gl version
			if (IsSuccess)
			{
		
			}
			else
			{

			}
			//todo: exit program
			GLFW_IsInit = true;
		}

		

		if(is_vulkan)
		{
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			window_data.window = glfwCreateWindow(window_data.width, window_data.height, window_data.title.c_str(), nullptr, nullptr);
			window_data.RenderContextData = std::make_shared<VulkanContext>(window_data.window);
			window_data.RenderContextData->Init(1,2);
		/*	vulkan_imgui_manager.VulkanInit(dynamic_cast<VulkanContext*>(window_data.RenderContextData.get())->GetVulkanType());
			vulkan_imgui_manager.Init(window_data.window);*/
		}
		else
		{
			window_data.window = glfwCreateWindow(window_data.width, window_data.height, window_data.title.c_str(), nullptr, nullptr);
			window_data.RenderContextData = std::make_shared<OpenglContext>(window_data.window);
			window_data.RenderContextData->Init(4,5);
		}

		
		if (window_data.window == NULL)
		{
			std::cout << "failed to Create window\n";
		}

		ShaderConfig shader_config{
			"shader",
			{
				E_StageType::VERTEX_SHADER,
				E_StageType::FRAGMENT_SHADER
			},
			2
		};

		window_data.RenderContextData->AddShader(&shader_config);
		window_data.RenderContextData->AddMesh("cube");



		//to get the glfw callback
		glfwSetWindowUserPointer(window_data.window, &window_data);
	}

	void Window::Shutdown()
	{
		if(is_vulkan)
		{
			
			//vkDestroyInstance(GetWindowData().RenderContextData->GetVulkanInstance(), nullptr);
			

		}
			glfwDestroyWindow(window_data.window);

		glfwTerminate();
	}

}
