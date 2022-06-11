#ifndef MESHMANAGER_H
#define MESHMANAGER_H
#include <memory>
#include <unordered_map>

#include <glm/matrix.hpp>

#include "../common/assets.h"

struct Vulkan_type;

namespace Renderer
{
	class VulkanShaderManager;
	class Mesh;

	class MeshManager
	{
	public:
		MeshManager() = default;
		virtual ~MeshManager() = default;
		virtual int CopyAssetData(const std::string&& mesh_name, const Asset::Mesh& mesh) = 0;
		virtual int DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat3& normal_matrix) =0;
		virtual int DeleteMeshData(const std::string& mesh_name) = 0;

		protected:
		std::unordered_map<std::string, std::unique_ptr<Mesh>> mesh_map;
	};


	class VulkanMeshManager : public MeshManager
	{
	public:
		VulkanMeshManager(Vulkan_type* vulkan_type, VulkanShaderManager* shader_manager);
		VulkanMeshManager(const VulkanMeshManager& mesh_manager) = delete;
		int CopyAssetData(const std::string&& mesh_name, const Asset::Mesh& mesh) override;
		int DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat3& normal_matrix) override;
		int DeleteMeshData(const std::string& mesh_name) override;


	private:
		Vulkan_type* vulkan_type;
		VulkanShaderManager* shader_manager;
	};
}


#endif
