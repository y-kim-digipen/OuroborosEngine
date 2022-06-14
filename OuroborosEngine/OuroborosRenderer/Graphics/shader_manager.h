#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H
#include <memory>
#include <unordered_map>

#include "shader.h"
struct Vulkan_type;

namespace Renderer
{
	class Shader;
	class ShaderManager
	{
	public:
		ShaderManager() = default;
		virtual ~ShaderManager() = default;
		virtual int AddShader(ShaderConfig * config) = 0;
		virtual int DeleteShader(ShaderConfig * config) = 0;
		virtual Shader* GetShader(const std::string& shader_name) = 0;

	protected:
		std::unordered_map<std::string, std::shared_ptr<Shader>> shader_map;
	};


	class VulkanShaderManager : public ShaderManager
	{
	public:
		VulkanShaderManager(Vulkan_type* vulkan_type);
		~VulkanShaderManager() = default;
		int AddShader(ShaderConfig* config) override;
		int DeleteShader(ShaderConfig* config) override;
		Shader* GetShader(const std::string& shader_name) override;
		friend class VulkanMeshManager;

	private:
		Vulkan_type* vulkan_type;
	};

}
#endif
