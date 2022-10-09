#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vulkan_material.h"
#include "vulkan_texture_manager.h"
#include "../context.h"

struct VulkanType;

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
		int DeferredEndFrame() override;
		int EndFrame() override;


		void UpdateViewportDescriptorSet(VkDescriptorSet descriptor_set, int dest_binding);

		std::unique_ptr<VulkanMeshManager> mesh_manager_;

		VulkanType* GetVulkanType();

		void DrawQueue() override;

	private:
		void CreateSurface();
		void CreateSwapChain();
		void RecreateSwapChain();
		DescriptorSet lightpass_set_;
	};

}

#endif // !VULKAN_CONTEXT_H
