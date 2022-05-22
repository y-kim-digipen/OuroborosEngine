#include <GL/glew.h>

#include "Context.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#include "shader.h"

namespace Renderer
{
	Context::Context(GLFWwindow* glfwwindow) : window(glfwwindow) {}


	void Context::SwapBuffer()
	{
		glfwSwapBuffers(window);
	}
	Context::~Context()
	{
	}
}
