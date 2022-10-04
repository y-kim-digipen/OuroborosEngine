#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vulkan_material.h"
#include "vulkan_texture_manager.h"
#include "../context.h"

struct Vulkan_type;

namespace Renderer {

	class VulkanContext : public Context {
	public:
		VulkanContext(GLFWwindow* window);
		void Init(int major, int minor) override;

		void InitGlobalData() override;
		void UpdateGlobalData() override;
		void BindGlobalData() override;

		void Shutdown() override;
				
		int BeginFrame() override;
		int EndFrame() override;

		std::unique_ptr<VulkanMeshManager> mesh_manager_;
	

		Vulkan_type* GetVulkanType();

		void DrawQueue() override;



		VulkanMaterial* new_material;
		VulkanMaterial* light_material;
		
	private:
		void CreateSurface();
		void CreateSwapChain();
		void RecreateSwapChain();
	};
}

#endif // !VULKAN_CONTEXT_H
