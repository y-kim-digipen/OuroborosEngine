#ifndef CONTEXT_H
#define CONTEXT_H


static bool is_vulkan = true;
struct GLFWwindow;

namespace Renderer
{
	struct ShaderConfig;

	class Context
	{
	public:
		Context(GLFWwindow* glfwwindow);
		virtual void Init(int major, int minor) = 0;
		void SwapBuffer();
		virtual void Shutdown() = 0;
		virtual int AddShader(ShaderConfig* config) = 0;

	protected:
		GLFWwindow* window;
	};


}

#endif // !CONTEXT_H