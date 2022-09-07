#include "vulkan_shader.h"

#include "spirv_helper.h"
#include "vulkan_pipeline.h"
#include "vulkan_buffer.h"

#include "SPIRV-Reflect/spirv_reflect.h"

#include <glm/matrix.hpp>
#include <glm/vector_relational.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <gtc/matrix_transform.hpp>

#include "../mesh.h"

namespace Renderer {


	struct MeshConstant
	{
		glm::mat4 model;
		glm::mat4 normal_matrix;
	};

	static void GetFilePath(const std::string& full_path, std::string& path_without_file_name)
	{
		size_t found = full_path.find_last_of("/\\");
		path_without_file_name = full_path.substr(0, found + 1);
	}

	void ReadFile(std::string& buffer,const std::string& filename)
	{
		std::ifstream file(filename);

		if (!file.is_open()) 
		{
			throw std::runtime_error("failed to open file!");
		}
		static bool isRecursiveCall = false;
		static std::string include_indentifier = "#include";

		std::string line_buffer;
		while (std::getline(file, line_buffer))
		{
			if (line_buffer.find(include_indentifier) != line_buffer.npos)
			{
				line_buffer.erase(0, include_indentifier.size());
				line_buffer.erase(0, 2);
				line_buffer.erase(line_buffer.size() - 1, 1);

				std::string path_of_this_file;
				GetFilePath(filename, path_of_this_file);
				line_buffer.insert(0, path_of_this_file);

				isRecursiveCall = true;
				
				std::string new_buffer;
				ReadFile(new_buffer,line_buffer);
				buffer += new_buffer + '\n';
				continue;
			}
			buffer += line_buffer + '\n';
		}

		if (!isRecursiveCall)
			buffer += '\0';

		file.close();

	}

	VulkanShader::VulkanShader(Vulkan_type* vulkan_type) :  vulkan_type(vulkan_type), device(&vulkan_type->device)
	{
		reload_next_frame = false;
	}

	VulkanShader::~VulkanShader()
	{
		ShutDown();
	}

	void VulkanShader::Init(ShaderConfig* config)
	{
		reload_next_frame = false;

		if (config != &this->config)
			this->config = *config;
		uint32_t stage_count = config->stage_count;

		
		std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};
		std::array<std::unordered_map<uint32_t, VkDescriptorSetLayoutBinding>, 4> layout_bindings_set;

		for (uint32_t i = 0; i < stage_count; ++i) {

			VkShaderModule shader_module{};
			VkPipelineShaderStageCreateInfo shader_stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };

			std::string shader_name = config->name;

			if (config->stages[i].type == VERTEX_SHADER) {
				shader_name.append(".vert");
				if (!CreateShaderModule(&shader_module, shader_name.c_str(), VK_SHADER_STAGE_VERTEX_BIT, push_constant_ranges, layout_bindings_set)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
				else {
					return;
				}
			}
			else if (config->stages[i].type == FRAGMENT_SHADER) {
				shader_name.append(".frag");
				if (!CreateShaderModule(&shader_module, shader_name.c_str(), VK_SHADER_STAGE_FRAGMENT_BIT, push_constant_ranges, layout_bindings_set)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
				else {
					return;
				}
			}

			shader_stage_create_infos.push_back(shader_stage_create_info);
		}

		Vulkan_PipelineBuilder pipeline_builder;
		
		for (uint32_t i = 0; i < max_set_count; ++i) {
			uint32_t binding_count = layout_bindings_set[i].size();

			std::vector<VkDescriptorSetLayoutBinding> bindings(binding_count);
			std::transform(layout_bindings_set[i].begin(), layout_bindings_set[i].end(), bindings.begin(), [](const std::pair<uint32_t, VkDescriptorSetLayoutBinding>& pair) {return pair.second; });

			VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };

			set_layout_create_info.bindingCount = binding_count;
			set_layout_create_info.pBindings = bindings.data();
			VkDescriptorSetLayout set_layout;
			VK_CHECK(vkCreateDescriptorSetLayout(device->handle, &set_layout_create_info, 0, &set_layout));
			descriptor_set_layouts[i] = set_layout;

			// set ubo descriptor set only for shader descriptor set #1
			if (binding_count != 0 && (i == 1)) {
				((VulkanUniformBuffer*)uniform_buffer_object.get())->SetupDescriptorSet(descriptor_set_layouts[i]);
			}
		}

		pipeline_builder.color_blend_attachment = VulkanInitializer::PipelineColorBlendAttachmentState();
		pipeline_builder.input_assembly = VulkanInitializer::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);

		VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

		//TODO: put in vertex_mesh class as a static
		VkVertexInputBindingDescription input_binding_description;
		input_binding_description.binding = 0;
		input_binding_description.stride = sizeof(Vertex);
		input_binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> input_attribute_descriptions(3);

		input_attribute_descriptions[0].binding = 0;
		input_attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		input_attribute_descriptions[0].location = 0;
		input_attribute_descriptions[0].offset = offsetof(Vertex, position);

		input_attribute_descriptions[1].binding = 0;
		input_attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		input_attribute_descriptions[1].location = 1;
		input_attribute_descriptions[1].offset = offsetof(Vertex, normal);

		input_attribute_descriptions[2].binding = 0;
		input_attribute_descriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
		input_attribute_descriptions[2].location = 2;
		input_attribute_descriptions[2].offset = offsetof(Vertex, uv);

		pipeline_vertex_input_state_create_info.pVertexAttributeDescriptions = input_attribute_descriptions.data();
		pipeline_vertex_input_state_create_info.pVertexBindingDescriptions = &input_binding_description;
		pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 1;
		pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = input_attribute_descriptions.size();

		pipeline_builder.vertex_input_info = pipeline_vertex_input_state_create_info;
		pipeline_builder.multisampling =  VulkanInitializer::PipelineMultisampleStateCreateInfo();
		pipeline_builder.rasterizer = VulkanInitializer::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		pipeline_builder.depth_stencil = VulkanInitializer::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS);

		pipeline_builder.viewport = { .x = 0.f, .y = 0.f, .width = static_cast<float>(vulkan_type->swapchain.extent.width)
									, .height = static_cast<float>(vulkan_type->swapchain.extent.height),.minDepth = 0.f, .maxDepth = 1.f };

		pipeline_builder.scissor = { .offset = {0,0},.extent = vulkan_type->swapchain.extent };

		pipeline_layout = pipeline_builder.BuildPipeLineLayout(device->handle, descriptor_set_layouts, max_set_count, push_constant_ranges.data(), push_constant_ranges.size());
		//build pipeline
		pipeline = pipeline_builder.BuildPipeLine(device->handle, vulkan_type->render_pass, shader_stage_create_infos);

		for (uint32_t i = 0; i < E_StageType::MAX_VALUE; ++i) {
			if (shader_stage_create_infos[i].module != VK_NULL_HANDLE)
				vkDestroyShaderModule(device->handle, shader_stage_create_infos[i].module, 0);
		}
	}

	void VulkanShader::Bind()
	{
		if (pipeline == VK_NULL_HANDLE) {
			vulkan_type->current_pipeline_layout = VK_NULL_HANDLE;
			return;
		}
		uint32_t current_frame = vulkan_type->current_frame;
		auto& frame_data = vulkan_type->frame_data[current_frame];

		vulkan_type->current_pipeline_layout = pipeline_layout;

		vkCmdBindPipeline(frame_data.command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		
		// bind shader descriptor set 1
		if(uniform_buffer_object)
			uniform_buffer_object->Bind();
	}

	void VulkanShader::ShutDown()
	{
		uniform_buffer_object.reset();
		uniform_buffer_object = nullptr;

		descriptor_data.clear();
		push_constant_ranges.clear();


		for (uint32_t i = 0; i < max_set_count; ++i) {
			vkDestroyDescriptorSetLayout(device->handle, descriptor_set_layouts[i], nullptr);
			descriptor_set_layouts[i] = VK_NULL_HANDLE;
		}

		if (pipeline_layout != VK_NULL_HANDLE)
			vkDestroyPipelineLayout(device->handle, pipeline_layout, nullptr);

		if (pipeline != VK_NULL_HANDLE)
			vkDestroyPipeline(device->handle, pipeline, nullptr);

		pipeline_layout = VK_NULL_HANDLE;
		pipeline = VK_NULL_HANDLE;
	}

	void VulkanShader::Reload()
	{
		if (device->handle != nullptr)
			vkDeviceWaitIdle(device->handle);

		ShutDown();
		Init(&config);
	}

	int VulkanShader::CreateShaderModule(VkShaderModule* out_shader_module, const char* file_name, VkShaderStageFlagBits shader_type, std::vector<VkPushConstantRange>& push_constant_ranges, std::array < std::unordered_map<uint32_t,VkDescriptorSetLayoutBinding>, 4>& layout_bindings_set)
	{
		std::string full_path = "shaders/";
		full_path.append(file_name);

		std::string shader_buffer;
		ReadFile(shader_buffer, full_path);
		const char* pshader = shader_buffer.c_str();
		std::vector<unsigned> shader_binary_code;


		if (!SpirvHelper::GLSLtoSPV(shader_type, pshader, shader_binary_code)) {
			std::cout << "Failed to convert GLSL to Spriv\n";
			return -1;
		}

		// Generate reflection data for a shader
		SpvReflectShaderModule refl_module;
		SpvReflectResult result = spvReflectCreateShaderModule(sizeof(uint32_t) * shader_binary_code.size(), shader_binary_code.data(), &refl_module);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		uint32_t descriptor_set_count = 0;
		spvReflectEnumerateDescriptorSets(&refl_module, &descriptor_set_count, 0);
		std::vector<SpvReflectDescriptorSet*> pdescriptor_sets(descriptor_set_count);
		spvReflectEnumerateDescriptorSets(&refl_module, &descriptor_set_count, pdescriptor_sets.data());

		for (uint32_t i_set = 0; i_set < descriptor_set_count; ++i_set) {

			const SpvReflectDescriptorSet& refl_set = *pdescriptor_sets[i_set];
	
			if (refl_set.set == 1 && (uniform_buffer_object.get() == nullptr)) {
				uniform_buffer_object = std::make_unique<VulkanUniformBuffer>(vulkan_type, refl_set.set);
			}

			for (uint32_t i_binding = 0; i_binding < refl_set.binding_count; ++i_binding) {
				
				const SpvReflectDescriptorBinding& refl_binding = *refl_set.bindings[i_binding];

				if (layout_bindings_set[refl_set.set].find(refl_binding.binding) != layout_bindings_set[refl_set.set].end()) {
					layout_bindings_set[refl_set.set][refl_binding.binding].stageFlags |= refl_module.shader_stage;
				}
				else {

					uint32_t descriptor_count = 1;

					for (uint32_t i_dim = 0; i_dim < refl_binding.array.dims_count; ++i_dim) {
						descriptor_count *= refl_binding.array.dims[i_dim];
					}

					layout_bindings_set[refl_set.set][refl_binding.binding] = {
						refl_binding.binding,
						(VkDescriptorType)refl_binding.descriptor_type,
						descriptor_count,
						(VkShaderStageFlags)refl_module.shader_stage,
						0,
					};

					// dont add ubo var for material & object & global
					if (refl_set.set == 1)
					{
						for (uint32_t i = 0; i < refl_binding.block.member_count; ++i) {

							DataType data_type = DataType::NONE;

							switch (refl_binding.block.members[i].type_description->op) {
							case SpvOp::SpvOpTypeMatrix:
								if (refl_binding.block.members[i].size == sizeof(glm::mat4))
									data_type = DataType::MAT4;
								else if (refl_binding.block.members[i].size == sizeof(glm::mat3))
									data_type = DataType::MAT3;

								break;
							case SpvOp::SpvOpTypeVector:

								if (refl_binding.block.members[i].size == sizeof(glm::vec4))
									data_type = DataType::FLOAT4;
								else if (refl_binding.block.members[i].size == sizeof(glm::vec3))
									data_type = DataType::FLOAT3;
								else if (refl_binding.block.members[i].size == sizeof(glm::vec2))
									data_type = DataType::FLOAT2;

								break;
							case SpvOp::SpvOpTypeFloat:
								data_type = DataType::FLOAT;
								break;
							case SpvOp::SpvOpTypeInt:
								data_type = DataType::INT;
								break;
							case SpvOp::SpvOpTypeBool:
								data_type = DataType::BOOL;
								break;
							default:
								DataType::NONE;
							}

							uniform_buffer_object->AddMember(
								std::string(refl_binding.block.name) + std::string(".") + refl_binding.block.members[i].name,
								data_type,
								refl_binding.block.members[i].size,
								refl_binding.block.members[i].offset
							);

						}

						((VulkanUniformBuffer*)(uniform_buffer_object.get()))->AddBinding(refl_binding.binding, refl_binding.block.size);
					}

					descriptor_data[refl_binding.name] = { refl_binding.set ,refl_binding.binding, descriptor_count, (VkDescriptorType)refl_binding.descriptor_type };
				}
			}
		}

		uint32_t push_constant_block_count = 0;
		spvReflectEnumeratePushConstantBlocks(&refl_module, &push_constant_block_count, 0);
		std::vector<SpvReflectBlockVariable*> ppush_constant_blocks(push_constant_block_count);
		spvReflectEnumeratePushConstantBlocks(&refl_module, &push_constant_block_count, ppush_constant_blocks.data());

		if (push_constant_block_count > 0) {
			VkPushConstantRange range{};
			range.offset = ppush_constant_blocks[0]->offset;
			range.size = ppush_constant_blocks[0]->size;
			range.stageFlags = refl_module.shader_stage;

			push_constant_ranges.push_back(range);
		}

		VkShaderModuleCreateInfo module_create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		//module_create_info.flags;
		module_create_info.codeSize = sizeof(uint32_t) * shader_binary_code.size();
		module_create_info.pCode = shader_binary_code.data();

		VK_CHECK(vkCreateShaderModule(device->handle, &module_create_info, 0, out_shader_module));

		// Destroy the reflection data when no longer required.
		spvReflectDestroyShaderModule(&refl_module);
		
		return 0;
	}


	void* VulkanShader::GetMemberVariable(const std::string& name)
	{
		if (uniform_buffer_object->member_vars.find(name) != uniform_buffer_object->member_vars.end()) {
			return (char*)uniform_buffer_object->data + uniform_buffer_object->member_vars[name].offset;
		}

		return nullptr;
	}

	void ShaderConfig::operator=(const ShaderConfig& config)
	{
		name = config.name;

		for (uint32_t i = 0; i < MAX_VALUE; ++i) {
			stages[i] = config.stages[i];
		}

		stage_count = config.stage_count;
	}
}
