#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "vulkan_type.inl"

#include <unordered_map>
#include <string>

#include "../shader.h"

namespace Renderer {

	enum class DataType;


	class Context;

	enum E_StageType {
		VERTEX_SHADER,
		FRAGMENT_SHADER,
		MAX_VALUE
	};

	struct ShaderStage {
		E_StageType type = MAX_VALUE;
	};

	struct ShaderConfig {
		const char* name;
		ShaderStage stages[MAX_VALUE];
		uint32_t stage_count;

		void operator=(const ShaderConfig& config);
	};


	constexpr int max_set_count = 4;

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

	class VulkanShader 
	{
	public:
		VulkanShader() = delete;
		VulkanShader(const VulkanShader& shader) = delete;
		VulkanShader(VulkanShader&& shader) = delete;

		VulkanShader(Vulkan_type* vulkan_type);
		~VulkanShader();

		void Init(ShaderConfig* config);
		void LightPassInit(ShaderConfig* config);
		//lightpass
		void Bind();
		//geometrypass
		void BindDeferred();
		void ShutDown();
		void Reload();
		void SetUniformValue(const char* name, void* data) {

			if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
				uniform_buffer_object->UpdateData(name, data);
				//uniform_buffer_object->Bind();
			}
		}
		void* GetMemberVariable(const std::string& name);

		std::unique_ptr<VulkanUniformBuffer> uniform_buffer_object;
		bool reload_next_frame;

		ShaderConfig config;
		VkPipelineLayout pipeline_layout;
	private:

		int CreateShaderModule(VkShaderModule* out_shader_module,  const char* file_name, VkShaderStageFlagBits shader_type, std::vector<VkPushConstantRange>& push_constant_ranges, std::array < std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>, 4>& layout_bindings_set);

		VulkanDevice* device;
		Vulkan_type* vulkan_type;

		VkDescriptorSetLayout descriptor_set_layouts[max_set_count];

		VkPipeline pipeline;

		std::unordered_map<std::string, DescriptorSetBindingData> descriptor_data;
		std::vector<VkPushConstantRange> push_constant_ranges;
	};
}
#endif // !VULKAN_SHADER_H

// uniform buffer
// total size
// member type, size, name, offset
