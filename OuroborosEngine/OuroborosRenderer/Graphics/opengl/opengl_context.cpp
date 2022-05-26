#include "opengl_context.h"

#include <iostream>
#include <GL/glew.h>
#include <gl/GL.h>
#include <GLFW/glfw3.h>

namespace Renderer
{
	void OpenglContext::Init(int major, int minor)
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
	void OpenglContext::Shutdown()
	{
	}
	int OpenglContext::AddShader(ShaderConfig* config)
	{
		return 0;
	}
	int OpenglContext::AddMesh(const char* mesh_name)
	{
		return 0;
	}
	void OpenglContext::DrawMeshes(const std::vector<const char*>& shaders_name, const std::vector<const char*>& meshes_name)
	{
	}
	void OpenglContext::DrawMesh(const char* shader_name, const char* mesh_name)
	{
	}
	int OpenglContext::BeginFrame()
	{
		return 0;
	}
	int OpenglContext::EndFrame()
	{
		glfwSwapBuffers(window);

		return 0;
	}
}