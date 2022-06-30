#ifndef VULKAN_IMGUI_MANAGER_H
#define VULKAN_IMGUI_MANAGER_H

#include <vulkan_core.h>

#include "../imgui_manager.h"
struct Vulkan_type;

namespace Renderer
{
	class VulkanImguiManager : public ImguiManager
	{
	public:
		VulkanImguiManager();
		void Init(GLFWwindow* window) override;
		void VulkanInit(Vulkan_type* vulkan_type);
		void BeginFrame() override;
		void EndFrame() override;
		void Shutdown() override;
		void Update() override;

		void GLFW_KeyboardCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
		void GLFW_MouseButtonCallback(GLFWwindow* window, int key, int action, int mods);
	private:
		Vulkan_type* vulkan_type;
		VkRenderPass imgui_render_pass;
	};
}


#endif