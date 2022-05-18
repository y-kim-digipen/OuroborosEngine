#include "vulkan_shader.h"

#include "vulkan_type.inl"
#include "spirv_helper.h"
#include "vulkan_pipeline.h"
#include "SPIRV-Reflect/spirv_reflect.h"
#include <string>
#include <iostream>
#include <fstream>

namespace Renderer {

	void ReadFile(std::string& buffer,const std::string& filename) {
		std::ifstream file(filename, std::ios::ate);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		buffer.resize(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();
	}

	int CreateShaderModule(VkShaderModule* out_shader_module,  std::string& file_name, VkShaderStageFlagBits shader_type);


	VulkanShader::VulkanShader() : Shader()
	{
		device = &vulkan_type.device;
	}

	VulkanShader::~VulkanShader()
	{
	}

	void VulkanShader::Init(ShaderConfig* config)
	{
		uint32_t stage_count = config->stage_count;
		
		std::vector<VkPipelineShaderStageCreateInfo> shader_stage_create_infos{};

		for (uint32_t i = 0; i < stage_count; ++i) {

			VkShaderModule shader_module{};
			VkPipelineShaderStageCreateInfo shader_stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };

			std::string shader_name = config->name;

			if (config->stages[i].type == VERTEX_SHADER) {
				shader_name.append(".vert");
				if (CreateShaderModule(&shader_module, shader_name, VK_SHADER_STAGE_VERTEX_BIT)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
			}
			else if (config->stages[i].type == FRAGMENT_SHADER) {
				shader_name.append(".frag");
				if (CreateShaderModule(&shader_module, shader_name, VK_SHADER_STAGE_FRAGMENT_BIT)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
			}

			shader_stage_create_infos.push_back(shader_stage_create_info);
		}


		////
		Vulkan_PipelineBuilder pipeline_builder;


		//TODO: Description need to implement

		pipeline_builder.color_blend_attachment = VulkanInitializer::PipelineColorBlendAttachmentState();
		pipeline_builder.input_assembly = VulkanInitializer::PipelineInputAssemblyStateCreateInfo(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
		pipeline_builder.vertex_input_info = VulkanInitializer::PipelineVertexInputStageCreateInfo();
		pipeline_builder.multisampling =  VulkanInitializer::PipelineMultisampleStateCreateInfo();
		pipeline_builder.rasterizer = VulkanInitializer::PipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL);
		pipeline_builder.depth_stencil = VulkanInitializer::DepthStencilCreateInfo(true, true, VK_COMPARE_OP_LESS_OR_EQUAL);

		pipeline_builder.viewport = { .x = 0.f, .y = 0.f, .width = static_cast<float>(vulkan_type.swap_chain.swap_chain_extent.width)
									, .height = static_cast<float>(vulkan_type.swap_chain.swap_chain_extent.height) };

		pipeline_builder.scissor = { .offset = {0,0},.extent = vulkan_type.swap_chain.swap_chain_extent };


		//build pipeline
		pipeline = pipeline_builder.BuildPipeLine(vulkan_type.device.handle, vulkan_type.render_pass);



		////

		for (uint32_t i = 0; i < E_StageType::MAX_VALUE; ++i) {
			if (shader_stage_create_infos[i].module != VK_NULL_HANDLE)
				vkDestroyShaderModule(vulkan_type.device.handle, shader_stage_create_infos[i].module, 0);
		}


	}

	int CreateShaderModule(VkShaderModule* out_shader_module, std::string& file_name, VkShaderStageFlagBits shader_type)
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
		SpvReflectShaderModule module;
		SpvReflectResult result = spvReflectCreateShaderModule(sizeof(uint32_t) * shader_binary_code.size(), shader_binary_code.data(), &module);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		// Enumerate and extract shader's input variables
		uint32_t var_count = 0;
		result = spvReflectEnumerateInputVariables(&module, &var_count, NULL);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);
		SpvReflectInterfaceVariable** input_vars =
			(SpvReflectInterfaceVariable**)malloc(var_count * sizeof(SpvReflectInterfaceVariable*));
		result = spvReflectEnumerateInputVariables(&module, &var_count, input_vars);
		assert(result == SPV_REFLECT_RESULT_SUCCESS);

		// Output variables, descriptor bindings, descriptor sets, and push constants
		// can be enumerated and extracted using a similar mechanism.

		uint32_t desciptor_set_count = 0;
		spvReflectEnumerateDescriptorSets(&module, &desciptor_set_count, 0);

		SpvReflectDescriptorSet** descriptor_sets =
			(SpvReflectDescriptorSet**)malloc(desciptor_set_count * sizeof(SpvReflectDescriptorSet*));

		spvReflectEnumerateDescriptorSets(&module, &desciptor_set_count, descriptor_sets);

		// Destroy the reflection data when no longer required.
		spvReflectDestroyShaderModule(&module);

		return 0;
	}
}
