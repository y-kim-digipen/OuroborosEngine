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
		ShaderManager(Vulkan_type* vulkan_type);
		~ShaderManager() = default;
		int AddShader(ShaderConfig* config);
		int DeleteShader(ShaderConfig* config);
		friend class MeshManager;

	private:
		std::unordered_map<const char*, std::unique_ptr<Shader>> shader_map;
		Vulkan_type* vulkan_type;
	};

}
#endif
