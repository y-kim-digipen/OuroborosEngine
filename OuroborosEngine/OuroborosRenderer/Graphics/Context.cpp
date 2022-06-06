#include <GL/glew.h>

#include "Context.h"

#include <cassert>
#include <iostream>
#include <vector>
#include <GLFW/glfw3.h>

#include "shader.h"

namespace Renderer
{
	Context::Context(GLFWwindow* glfwwindow) : window(glfwwindow), material_manager() {}


	void Context::InitGlobalData()
	{
	}

	void Context::UpdateGlobalData()
	{
	}

	void Context::SwapBuffer()
	{
		glfwSwapBuffers(window);
	}
	int Context::BeginFrame()
	{
		return 0;
	}

	int Context::EndFrame()
	{
		return 0;
	}

	void Context::AddDrawQueue(TransformComponent* transform, MaterialComponent* material, MeshComponent* mesh,
		ShaderComponent* shader)
	{
		draw_queue.push({ transform,material, mesh, shader });
	}



	Context::~Context()
	{
	}
}
