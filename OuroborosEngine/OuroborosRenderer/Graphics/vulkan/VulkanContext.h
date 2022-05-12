#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "../context.h"

namespace Renderer {
	class VulkanContext : public Context {
	public:
		VulkanContext(GLFWwindow* window) :Context(window) {}
		void Init(int major, int minor) override;
		void Shutdown() override;
	private:
		void CreateSurface();
		void CreateSwapChain();
	};
}

#endif // !VULKAN_CONTEXT_H