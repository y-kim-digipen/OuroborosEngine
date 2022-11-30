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

	enum E_ShaderPass {
		DEFERRED_PASS,
		LIGHT_PASS,
		SSR_PASS
	};

	struct ShaderStage {
		E_StageType type = MAX_VALUE;
	};

	struct ShaderConfig {
		const char* name;
		ShaderStage stages[MAX_VALUE];
		uint32_t stage_count;
		bool use_built_in_quad;
		//TODO: renderpass
		//E_ShaderPass pass_type; 

		void operator=(const ShaderConfig& config);
	};


	constexpr int max_set_count = 4;

	struct DescriptorSetBindingData
	{
		uint32_t set;
		uint32_t binding;
		uint32_t count;
		VkDescriptorType type;
		VkShaderStageFlags flags;
	};

	struct BindingBlockMemberData
	{
		uint32_t binding_num;
		DataType type;
		uint32_t size;
		uint32_t offset;
		const char* texture_name = "";
	};

	class VulkanShader
	{
	public:
		VulkanShader() = delete;
		VulkanShader(const VulkanShader& shader) = delete;
		VulkanShader(VulkanShader&& shader) = delete;

		VulkanShader(VulkanType* vulkan_type);
		~VulkanShader();

		void Init(ShaderConfig* config, VkRenderPass renderpass = VK_NULL_HANDLE);
		//lightpass
		void Bind();
		//geometrypass
		void ShutDown();
		void Reload();

		void SetUniformValue(const char* name, void* data) {

			if (binding_block_members.find(name) != binding_block_members.end()) {
				uint32_t binding_num = binding_block_members[name].binding_num;
				if (uniform_buffer_objects.find(binding_num) != uniform_buffer_objects.end()) {
					uniform_buffer_objects[binding_num]->UpdateData(name, data);
				}
			}
		}

		void SetUniformTexture(const char* name, std::shared_ptr<VulkanTexture> p_texture) {
			if (binding_block_members.find(name) != binding_block_members.end()) {
				uint32_t binding_num = binding_block_members[name].binding_num;
				if (uniform_texture_objects.find(binding_num) != uniform_texture_objects.end()) {
					uniform_texture_objects[binding_num] = p_texture;
					shader_set.AddBinding(binding_num, p_texture.get());
				}
			}
		}

		//TODO: texture
		//void SetUniformValue(const char* name, VulkanTexture* texture);

		void* GetMemberVariable(const std::string& name, uint32_t binding_num = -1);
		std::shared_ptr<VulkanTexture> GetTextureVariable(const std::string& name, uint32_t binding_num = -1);

		DescriptorSet shader_set; // set num 1
		std::map<uint32_t, std::unique_ptr<VulkanUniformBuffer>> uniform_buffer_objects; // binding block ubo
		std::map<uint32_t, std::shared_ptr<VulkanTexture>> uniform_texture_objects;
		//bool reload_next_frame;

		ShaderConfig config;
		VkPipelineLayout pipeline_layout;
		VkPipeline pipeline;

		//VkDescriptorSet descriptor_sets[max_set_count];

		// first = binding_num, second = member_data
		std::map<std::string, BindingBlockMemberData> binding_block_members;

	private:

		int CreateShaderModule(VkShaderModule* out_shader_module, const char* file_name, VkShaderStageFlagBits shader_type, std::vector<VkPushConstantRange>& push_constant_ranges, std::array < std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>, 4>& layout_bindings_set);

		VulkanDevice* device;
		VulkanType* vulkan_type;

		VkDescriptorSetLayout descriptor_set_layouts[max_set_count];
		std::unordered_map<std::string, DescriptorSetBindingData> descriptor_data;
		std::vector<VkPushConstantRange> push_constant_ranges;
		std::vector<VkVertexInputAttributeDescription> input_attribute_descriptions;
		std::vector<VkPipelineColorBlendAttachmentState> pipeline_color_blend_attachments;
		std::shared_ptr<VulkanTexture> default_texture;

		bool use_shader_set = false;
	};
}
#endif // !VULKAN_SHADER_H

// uniform buffer
// total size
// member type, size, name, offset
