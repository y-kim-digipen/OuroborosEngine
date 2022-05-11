#ifndef CONTEXT_H
#define CONTEXT_H


static bool is_vulkan = true;
struct GLFWwindow;

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