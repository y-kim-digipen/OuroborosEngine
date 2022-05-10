#ifndef CONTEXT_H
#define CONTEXT_H


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
		virtual void Init(int major, int minor) = 0;
		void SwapBuffer();

	protected:
		GLFWwindow* window;
	};


}

#endif // !CONTEXT_H