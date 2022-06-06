#define GLFW_INCLUDE_VULKAN 
#include <GLFW/glfw3.h>
#include "Window.h"
#include "iostream"
#include "vulkan/vulkan_context.h"
#include "opengl/opengl_context.h"

#include "shader.h"

//TODO: temp header
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm.hpp>

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

	void Window::BeginFrame()
	{
		glfwPollEvents();
		window_data.RenderContextData->BeginFrame();
		vulkan_imgui_manager.BeginFrame();
	}

	void Window::Update()
	{
		//TODO: update global_data
		VulkanContext* vulkan_context = (VulkanContext*)window_data.RenderContextData.get();

		vulkan_imgui_manager.Update();

		// bind global data
		window_data.RenderContextData->UpdateGlobalData();
	
		//for(const auto& shader : vulkan_context->shader_manager_)
		for (shader) {
			shader.bind(); // bind pipeline & descriptor set 1 bind
			for (material : material_system.get_materai())
			{
				material.bind(); // descriptor set 2 bind
				for (object : registered_objects) {
					object.constant_bind(); // push_constant
				}
			}
		}

		window_data.RenderContextData->BindGlobalData();


		// for each shader bind pipeline ( bind shader descriptor set )
			// for each material bind (set = 2)
				// for each object (set = 3 or might be dynamic material )
					// bind push_constant (model, normal)
					

		//TODO : make close
		if (glfwWindowShouldClose(window_data.window))
		{
			
		}
		if (glfwGetKey(window_data.window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		{
		
		}
	}

	void Window::EndFrame()
	{
		vulkan_imgui_manager.EndFrame();
		window_data.RenderContextData->EndFrame();

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
					//refl_binding.resource_type


			window_data.window = glfwCreateWindow(window_data.width, window_data.height, window_data.title.c_str(), nullptr, nullptr);
			window_data.RenderContextData = std::make_shared<VulkanContext>(window_data.window);
			window_data.RenderContextData->Init(1,2);
			vulkan_imgui_manager.VulkanInit(dynamic_cast<VulkanContext*>(window_data.RenderContextData.get())->GetVulkanType());
			vulkan_imgui_manager.Init(window_data.window);
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



		//window_data.RenderContextData->AddMesh("suzanne");



		//to get the glfw callback
		glfwSetWindowUserPointer(window_data.window, &window_data);
	}

	void Window::Shutdown()
	{
		if(is_vulkan)
		{
			window_data.RenderContextData->Shutdown();
			//vkDestroyInstance(GetWindowData().RenderContextData->GetVulkanInstance(), nullptr);
			

		}
			glfwDestroyWindow(window_data.window);

		glfwTerminate();
	}

}
