#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "../shader.h"

struct VulkanDevice;

namespace Renderer {
	class VulkanShader : public Shader {
	public:
		VulkanShader();
		~VulkanShader() override;

		void Init(ShaderConfig* config) override;

	private:
		VulkanDevice* device;


	};
}
#endif // !VULKAN_SHADER_H

