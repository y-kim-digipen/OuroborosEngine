#ifndef CONTEXT_H
#define CONTEXT_H

#include <unordered_map>
#include <memory>

static bool is_vulkan = true;
struct GLFWwindow;

namespace Renderer
{
	struct ShaderConfig;
	class Shader;

	class Context
	{
	public:
		Context(GLFWwindow* glfwwindow);
		virtual void Init(int major, int minor) = 0;
		void SwapBuffer();
		virtual void Shutdown() = 0;
		virtual int AddShader(ShaderConfig* config) = 0;

		virtual ~Context();

	protected:
		GLFWwindow* window;
		std::unordered_map<const char*, std::unique_ptr<Shader>> shader_map;
	};


}

#endif // !CONTEXT_H