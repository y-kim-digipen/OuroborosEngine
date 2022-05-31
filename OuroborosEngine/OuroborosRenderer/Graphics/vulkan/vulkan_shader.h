#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "vulkan_type.inl"

#include <unordered_map>
#include <string>

#include "../shader.h"

namespace Renderer {

	enum class DataType;

	struct DescriptorSetBindingData
	{
		uint32_t set;
		uint32_t binding;
		uint32_t count;
		VkDescriptorType type;	
	};

	struct DescriptorSetLayoutData {
		std::string name;
		DataType type;
		uint32_t size;
		uint32_t offset;
	};

	class VulkanShader : public Shader
	{
	public:
		VulkanShader() = delete;
		VulkanShader(Vulkan_type* vulkan_type);
		~VulkanShader() override;

		void Init(ShaderConfig* config) override;
		void Bind() override;
	private:

		int CreateShaderModule(VkShaderModule* out_shader_module,  const char* file_name, VkShaderStageFlagBits shader_type, std::vector<VkPushConstantRange>& push_constant_ranges, std::array < std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>, 4>& layout_bindings_set);

		VulkanDevice* device;
		Vulkan_type* vulkan_type;

		VkDescriptorSetLayout descriptor_set_layouts[4];

		uint32_t set_layout_count;
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;

		std::unordered_map<std::string, DescriptorSetBindingData> descriptor_data;

		bool use_global_data;
	};
}
#endif // !VULKAN_SHADER_H

// uniform buffer
// total size
// member type, size, name, offset
