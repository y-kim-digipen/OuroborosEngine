#ifndef OPENGL_CONTEXT_H
#define OPENGL_CONTEXT_H

#include "../context.h"


namespace Renderer
{
	class OpenglContext : public Context
	{
	public:
		OpenglContext(GLFWwindow* glfwwindow) : Context(glfwwindow) {}
		void Init(int major, int minor)  override;
		void InitGlobalData() override;
		void UpdateGlobalData() override;
		void BindGlobalData() override;

		void Shutdown() override;
		void InitGlobalData() override;
		int BeginFrame() override;
		int EndFrame() override;

	private:


	};
}

#endif // !OPENGL_CONTEXT_H


