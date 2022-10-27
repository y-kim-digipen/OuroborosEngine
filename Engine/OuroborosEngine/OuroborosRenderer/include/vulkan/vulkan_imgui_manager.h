#ifndef VULKAN_IMGUI_MANAGER_H
#define VULKAN_IMGUI_MANAGER_H

#include <vulkan/vulkan_core.h>

#include "../imgui_manager.h"
struct VulkanType;

namespace Renderer
{
	class VulkanImguiManager : public ImguiManager
	{
	public:
		VulkanImguiManager();
		void Init(GLFWwindow* window) override;
		void VulkanInit(VulkanType* vulkan_type);
		void BeginFrame() override;
		void EndFrame() override;
		void Shutdown() override;
		void Update() override;

		void GLFW_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GLFW_MouseButtonCallback(GLFWwindow* window, int key, int action, int mods);
	private:
		VulkanType* vulkan_type;
		VkRenderPass imgui_render_pass;
	};
}


#endif