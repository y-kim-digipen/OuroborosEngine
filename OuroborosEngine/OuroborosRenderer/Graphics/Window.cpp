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
	//TODO: temp camera & global_ubo code
	struct camera_data {
		glm::vec3 position;
		glm::mat4 projection;
		glm::mat4 view;
	};
	static camera_data camera;


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
	
		//TODO: update global_data
		camera.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_data.width) / window_data.height, 0.1f, 100.0f);
		camera.view = glm::lookAt(camera.position, glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

		window_data.RenderContextData->BeginFrame();
		vulkan_imgui_manager.BeginFrame();
		vulkan_imgui_manager.Update();

		// for each shader bind pipeline
			// for each material bind (set = 1)
				// for each object bind push_constant (model, normal)

		window_data.RenderContextData->DrawMesh("shader", "suzanne");
		vulkan_imgui_manager.EndFrame();
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

		camera = {glm::vec3(0.0f,0.0f, -10.0f)};
		camera.projection = glm::perspective(glm::radians(45.0f), static_cast<float>(window_data.width) / window_data.height, 0.1f, 100.0f);
		camera.view = glm::lookAt(camera.position, glm::vec3(0.0, 0.0, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f));

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

		ShaderConfig shader_config{
			"shader",
			{
				E_StageType::VERTEX_SHADER,
				E_StageType::FRAGMENT_SHADER
			},
			2
		};

		window_data.RenderContextData->AddShader(&shader_config);
		window_data.RenderContextData->AddMesh("suzanne");



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
