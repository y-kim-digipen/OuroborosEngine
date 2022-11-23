#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include <memory>
#include <unordered_map>

#include "vulkan/vulkan_shader.h"
struct VulkanType;

namespace Renderer
{
	class VulkanShaderManager 
	{
	public:
		VulkanShaderManager(VulkanType* vulkan_type);
		~VulkanShaderManager() = default;
		int AddShader(ShaderConfig* config) ;
		int DeleteShader(ShaderConfig* config) ;
		void Cleanup();
		VulkanShader* GetShader(const std::string& shader_name);
		friend class VulkanMeshManager;

	private:
		VulkanType* vulkan_type;
		std::unordered_map<std::string, std::shared_ptr<VulkanShader>> shader_map;
	};

}
#endif
