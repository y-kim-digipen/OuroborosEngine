#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "../context.h"

struct Vulkan_type;

namespace Renderer {

	class VulkanContext : public Context {
	public:
		VulkanContext(GLFWwindow* window) : Context(window), shader_manager_(GetVulkanType()), mesh_manager_(GetVulkanType(), &shader_manager_)
		{
		};
		void Init(int major, int minor) override;
		void Shutdown() override;
				
		int BeginFrame() override;
		int EndFrame() override;
		void InitGlobalData() override;

		VulkanShaderManager shader_manager_;
		VulkanMeshManager mesh_manager_;

		Vulkan_type* GetVulkanType();
	private:
		void CreateSurface();
		void CreateSwapChain();
		void RecreateSwapChain();
	};
}

#endif // !VULKAN_CONTEXT_H
