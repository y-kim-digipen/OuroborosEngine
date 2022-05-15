#include "vulkan_shader.h"

#include "vulkan_type.inl"
#include "spirv_helper.h"

#include <string>
#include <iostream>
#include <fstream>

namespace Renderer {

	std::vector<char> ReadFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);
		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	int CreateShaderModule(VkShaderModule* out_shader_module, const char* file_path, VkShaderStageFlagBits shader_type);

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

			if (config->stages[i].type == VERTEX_SHADER) {
				if (CreateShaderModule(&shader_module, config->name, VK_SHADER_STAGE_VERTEX_BIT)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
			}
			else if (config->stages[i].type == FRAGMENT_SHADER) {
				if (CreateShaderModule(&shader_module, config->name, VK_SHADER_STAGE_FRAGMENT_BIT)) {
					shader_stage_create_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
					shader_stage_create_info.module = shader_module;
					shader_stage_create_info.pName = "main";
				}
			}

			shader_stage_create_infos.push_back(shader_stage_create_info);
		}


		for (uint32_t i = 0; i < E_StageType::MAX_VALUE; ++i) {
			if (shader_stage_create_infos[i].module != VK_NULL_HANDLE)
				vkDestroyShaderModule(vulkan_type.device.handle, shader_stage_create_infos[i].module, 0);
		}
	}

	int CreateShaderModule(VkShaderModule* out_shader_module, const char* file_path, VkShaderStageFlagBits shader_type)
	{
		std::string full_path="shaders/";
		full_path.append(file_path);

		const char* pshader = ReadFile(full_path).data();

		std::vector<unsigned> shader_binary_code;

		if (!SpirvHelper::GLSLtoSPV(shader_type, pshader, shader_binary_code)) {
			std::cout << "Failed to convert GLSL to Spriv\n";
			return -1;
		}
		
		VkShaderModuleCreateInfo create_info{ VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO };
		create_info.codeSize = shader_binary_code.size();
		create_info.pCode = shader_binary_code.data();

		return 0;
	}
}
