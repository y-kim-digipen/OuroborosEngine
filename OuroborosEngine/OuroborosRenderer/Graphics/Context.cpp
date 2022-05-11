#include <GL/glew.h>

#include <vulkan.h>
#include "Context.h"

#include <cassert>
#include <iostream>
#include <vector>


namespace Renderer
{
	Context::Context(GLFWwindow* glfwwindow) : window(glfwwindow) {}


	void Context::SwapBuffer()
	{
		glfwSwapBuffers(window);
	}


}
