#pragma once
#define GLFW_INCLUDE_VULKAN 
#include <string>
#include <GLFW/glfw3.h>

static bool is_vulkan = true;

namespace Renderer
{
	class Context
	{
	public:
		Context(GLFWwindow* glfwwindow);
		virtual void Init(int major, int minor);
		void SwapBuffer();

	protected:
		GLFWwindow* window;
	};


}