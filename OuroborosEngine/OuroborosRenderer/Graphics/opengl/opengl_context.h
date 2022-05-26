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
		void Shutdown() override;

		int AddShader(ShaderConfig* config) override;
		int AddMesh(const char* mesh_name) override;

		void DrawMeshes(const std::vector<const char*>& shaders_name, const std::vector<const char*>& meshes_name) override;
		void DrawMesh(const char* shader_name, const char* mesh_name) override;


		int BeginFrame() override;
		int EndFrame() override;

	private:


	};
}

#endif // !OPENGL_CONTEXT_H

