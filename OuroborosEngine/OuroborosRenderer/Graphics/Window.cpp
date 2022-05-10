#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>
#include "Window.h"
#include "iostream"


namespace Renderer
{
	Window::Window(const WindowProperties& window_properties)
	{
			// make unique pointer
			Init(window_properties);
	}

	void Window::Update()
	{
		glfwPollEvents();
		if(!is_vulkan)
		{
			window_data.RenderContextData->SwapBuffer();
		}

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
				if(is_vulkan)
				{
					Context::SetupVulkan(1,2, window_data.title);
				}
				else
				{
					Context::SetupOpenGLVersion(4, 0);
				}
			}
			else
			{

			}
			//todo: exit program
			GLFW_IsInit = true;
		}

		window_data.window = glfwCreateWindow(window_data.width, window_data.height, window_data.title.c_str(), nullptr, nullptr);

		if (window_data.window == NULL)
		{
			std::cout << "failed to Create window\n";
		}
		window_data.RenderContextData = std::make_shared<Context>(window_data.window);
		window_data.RenderContextData->Init();


		
		//to get the glfw callback
		glfwSetWindowUserPointer(window_data.window, &window_data);
	}

	void Window::Shutdown()
	{
		if(is_vulkan)
		{
			vkDestroyInstance(GetWindowData().RenderContextData->GetVulkanInstance(), nullptr);

			glfwDestroyWindow(window_data.window);
		}

		glfwTerminate();
	}

}