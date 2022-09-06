#ifndef MESHMANAGER_H
#define MESHMANAGER_H
#include <memory>
#include <unordered_map>

#include <glm/matrix.hpp>

#include "../common/assets.h"
#include "vulkan/vulkan_mesh.h"

struct Vulkan_type;

namespace Renderer
{
	class VulkanShaderManager;
	class Mesh;



	class VulkanMeshManager 
	{
	public:
		VulkanMeshManager(Vulkan_type* vulkan_type);
		VulkanMeshManager(const VulkanMeshManager& mesh_manager) = delete;
		int CopyAssetData(const std::string&& mesh_name, const Asset::Mesh& mesh);
		int DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat3& normal_matrix) ;
		int DeleteMeshData(const std::string& mesh_name);


	private:
		Vulkan_type* vulkan_type;
		std::unordered_map<std::string, std::unique_ptr<VulkanMesh>> mesh_map;
	};
}


#endif
