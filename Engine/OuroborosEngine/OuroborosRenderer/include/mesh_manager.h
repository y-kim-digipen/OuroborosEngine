#ifndef MESHMANAGER_H
#define MESHMANAGER_H
#include <memory>
#include <unordered_map>

#include <glm/matrix.hpp>

#include <common/assets.h>
#include "vulkan/vulkan_mesh.h"

struct VulkanType;

namespace Renderer
{

	class VulkanMeshManager 
	{
	public:
		VulkanMeshManager(VulkanType* vulkan_type);
		VulkanMeshManager(const VulkanMeshManager& mesh_manager) = delete;
		int CopyAssetData(const std::string&& model_name, const Asset::Mesh& mesh);
		int DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat4& normal_matrix) ;
		int DeleteMeshData(const std::string& model_name);
		void Cleanup();
	private:
		VulkanType* vulkan_type;
		std::unordered_map<std::string, VulkanMesh*> mesh_map;
		std::unordered_map<std::string ,Model*> model_list;
	};
}


#endif
