#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include <memory>
#include <unordered_map>

#include "vulkan/vulkan_shader.h"
struct Vulkan_type;

namespace Renderer
{
	class Shader;

	class VulkanShaderManager 
	{
	public:
		VulkanShaderManager(Vulkan_type* vulkan_type);
		~VulkanShaderManager() = default;
		int AddShader(ShaderConfig* config) ;
		int DeleteShader(ShaderConfig* config) ;
		VulkanShader* GetShader(const std::string& shader_name);
		friend class VulkanMeshManager;

	private:
		Vulkan_type* vulkan_type;
		std::unordered_map<std::string, std::shared_ptr<VulkanShader>> shader_map;
	};

}
#endif
