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
	protected:
		GLFWwindow* window;
	};


}