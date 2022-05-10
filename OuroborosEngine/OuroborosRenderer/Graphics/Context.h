#pragma once
#define GLFW_INCLUDE_VULKAN 
#include <string>
#include <vulkan.h>
#include <GLFW/glfw3.h>

static bool is_vulkan = true;

namespace Renderer
{
	class Context
	{
	public:
		static void SetupOpenGLVersion(int major, int minor);
		static void SetupVulkan(int major, int minor, std::string application_name, std::string engine_name = "Ouroboros");

		Context(GLFWwindow* glfwwindow);
		void Init();
		void SwapBuffer();
		VkInstance GetVulkanInstance() const { return instance; }
		VkSurfaceKHR GetVulkanSurface() const { return vk_surface; }
	private:
		struct VulkanInfo
		{
			VkApplicationInfo vk_application_info;
			VkInstanceCreateInfo vk_instance_create_info;
		};

		GLFWwindow* window;
		VkInstance instance;
		VkSurfaceKHR vk_surface;
		static VulkanInfo vulkan_info;
	};


}