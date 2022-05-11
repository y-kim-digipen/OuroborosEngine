#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "../context.h"


namespace Renderer
{
	class OpenGL_Context : public Context
	{
	public:
		OpenGL_Context(GLFWwindow* glfwwindow) : Context(glfwwindow) {}
		void Init(int major, int minor)  override;
	private:


	};
}

#endif // !OPENGL_CONTEXT_H


