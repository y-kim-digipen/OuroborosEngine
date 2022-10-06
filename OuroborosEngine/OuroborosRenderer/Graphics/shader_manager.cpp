#include "shader_manager.h"

#include <iostream>

#include "vulkan/vulkan_shader.h"

namespace Renderer
{
	VulkanShaderManager::VulkanShaderManager(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	int VulkanShaderManager::AddShader(ShaderConfig* config)
	{
		if (shader_map.find(config->name) != shader_map.end())
		{
			return -1;
		}
		else
		{
			shader_map[config->name] = std::make_unique<VulkanShader>(vulkan_type);
			shader_map[config->name]->LightPassInit(config);
		}

		return 0;
	}

	int VulkanShaderManager::DeleteShader(ShaderConfig* config)
	{
		if(auto iter = shader_map.find(config->name); iter != shader_map.end())
		{
			iter->second.reset();
			shader_map.erase(iter);
			return 0;
		}
		else
		{
			std::cout << config->name << " doesn't exist! " << std::endl;
			return -1;
		}
	}

	void VulkanShaderManager::Cleanup()
	{
		shader_map.clear();
	}

	VulkanShader* VulkanShaderManager::GetShader(const std::string& shader_name)
	{
		if(auto iter = shader_map.find(shader_name); iter != shader_map.end())
		{
			return iter->second.get();
		}
		else
		{
			std::cout << shader_name << " doesn't exist! " << std::endl;
		}

	}
}
