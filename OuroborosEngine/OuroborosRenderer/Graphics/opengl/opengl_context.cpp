#include "opengl_context.h"

#include <iostream>
#include <GL/glew.h>


namespace Renderer
{
	void OpenGL_Context::Init(int major, int minor)
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


		glfwMakeContextCurrent(window);
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cout << "Failed to init GLEW" << std::endl;
		}
		std::cout << "OpenGL Info :" << std::endl;
		std::cout << "OpenGL Version :" << glGetString(GL_VERSION) << std::endl;
		std::cout << "OpenGL Renderer:" << glGetString(GL_RENDERER) << std::endl;
		std::cout << "-------------------------------------------------" << std::endl;
	}
}
