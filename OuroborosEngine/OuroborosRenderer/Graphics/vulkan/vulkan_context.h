#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "../context.h"

namespace Renderer {
	class Vulkan_Context : public Context {
	public:
		void Init(int major, int minor) override;
	private:
		

	};
}

#endif // !VULKAN_CONTEXT_H
