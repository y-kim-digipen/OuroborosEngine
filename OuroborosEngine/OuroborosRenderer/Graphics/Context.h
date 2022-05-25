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
		virtual int BeginFrame();
		virtual void DrawMeshes(const std::vector<const char*>& shaders_name, const std::vector<const char*>& meshes_name) = 0;
		virtual void DrawMesh(const char* shader_name, const char* mesh_name) = 0;
		virtual int EndFrame();

		virtual ~Context();

	protected:
		GLFWwindow* window;
		std::unordered_map<const char*, std::unique_ptr<Shader>> shader_map;
		std::unordered_map<const char*, std::unique_ptr<Mesh>> mesh_map;
	};


}

#endif // !CONTEXT_H