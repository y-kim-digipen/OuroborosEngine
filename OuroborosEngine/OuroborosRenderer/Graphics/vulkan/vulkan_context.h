#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "../context.h"

struct Vulkan_type;

namespace Renderer {

	class VulkanContext : public Context {
	public:
		VulkanContext(GLFWwindow* window) :Context(window) {}
		void Init(int major, int minor) override;
		void Shutdown() override;
		int AddShader(ShaderConfig* config) override;
		int AddMesh(const char* mesh_name) override;
		void DrawMeshes(const std::vector<const char*>& shaders_name, const std::vector<const char*>& meshes_name) override;
		void DrawMesh(const char* shader_name, const char* mesh_name) override;
		int BeginFrame() override;
		int EndFrame() override;
		Vulkan_type* GetVulkanType();
	private:
		void CreateSurface();
		void CreateSwapChain();
		void RecreateSwapChain();
	};
}

#endif // !VULKAN_CONTEXT_H
