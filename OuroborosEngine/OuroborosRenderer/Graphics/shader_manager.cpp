#include "shader_manager.h"

#include <iostream>

#include "vulkan/vulkan_shader.h"

namespace Renderer
{
	ShaderManager::ShaderManager(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	int ShaderManager::AddShader(ShaderConfig* config)
	{
		if (shader_map.find(config->name) != shader_map.end())
		{
			return -1;
		}
		else
		{
			shader_map[config->name] = std::make_unique<VulkanShader>(vulkan_type);
			shader_map[config->name]->Init(config);
		}

		return 0;
	}

	int ShaderManager::DeleteShader(ShaderConfig* config)
	{
		if(auto iter = shader_map.find(config->name); iter != shader_map.end())
		{
			iter->second.reset();
			shader_map.erase(iter);
			return 0;
		}
		else
		{
			std::cout << config->name << "doesn't exist! " << std::endl;
			return -1;
		}


	}
}
