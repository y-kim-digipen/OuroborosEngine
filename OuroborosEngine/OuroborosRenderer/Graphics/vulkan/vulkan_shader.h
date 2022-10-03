#ifndef VULKAN_SHADER_H
#define VULKAN_SHADER_H

#include "vulkan_type.inl"
#include "../shader.h"
#include "vulkan_descriptor_set.h"

#include <map>
#include <unordered_map>
#include <string>


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

	struct BindingBlockMemberData
	{
		uint32_t binding_num;
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

		VulkanShader(VulkanType* vulkan_type);
		~VulkanShader();

		void Init(ShaderConfig* config);
		void Bind();
		void ShutDown();
		void Reload();

		void SetUniformValue(const char* name, void* data) {

			/*
			if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
				if (uniform_buffer_object->member_vars[name].type != DataType::SAMPLER2D)
					uniform_buffer_object->UpdateData(name, data);
			}
			*/
		}

		//TODO: texture
		//void SetUniformValue(const char* name, VulkanTexture* texture);

		void* GetMemberVariable(const std::string& name, uint32_t binding_num = -1);

		DescriptorSet shader_set; // set num 1
		std::map<uint32_t ,std::unique_ptr<VulkanUniformBuffer>> uniform_buffer_objects; // binding block ubo
		std::map<uint32_t ,std::shared_ptr<VulkanTexture>> uniform_texture_objects;
		bool reload_next_frame;

		ShaderConfig config;
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;

		//VkDescriptorSet descriptor_sets[max_set_count];
		
		// first = binding_num, second = member_data
		std::map<std::string, BindingBlockMemberData> binding_block_members;

	private:

		int CreateShaderModule(VkShaderModule* out_shader_module,  const char* file_name, VkShaderStageFlagBits shader_type, std::vector<VkPushConstantRange>& push_constant_ranges, std::array < std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>, 4>& layout_bindings_set);

		VulkanDevice* device;
		VulkanType* vulkan_type;

		VkDescriptorSetLayout descriptor_set_layouts[max_set_count];
		std::unordered_map<std::string, DescriptorSetBindingData> descriptor_data;
		std::vector<VkPushConstantRange> push_constant_ranges;
	};
}
#endif // !VULKAN_SHADER_H

// uniform buffer
// total size
// member type, size, name, offset
