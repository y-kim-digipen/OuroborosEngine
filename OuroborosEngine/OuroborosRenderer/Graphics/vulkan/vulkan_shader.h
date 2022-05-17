#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include <vulkan_core.h>

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

		VkDescriptorSetLayout descriptor_set_layout;

		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;


	};
}
#endif // !VULKAN_SHADER_H

