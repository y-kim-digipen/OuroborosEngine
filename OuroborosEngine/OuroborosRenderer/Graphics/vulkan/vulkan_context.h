#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "../context.h"

namespace Renderer {
	class Vulkan_Context : public Context {
	public:
		Vulkan_Context(GLFWwindow* window) :Context(window) {}
		void Init(int major, int minor) override;
	private:
		void CreateSurface();

	};
}

#endif // !VULKAN_CONTEXT_H
