#ifndef MESHMANAGER_H
#define MESHMANAGER_H
#include <memory>
#include <unordered_map>

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
		virtual int AddMesh(const std::string& mesh_name) = 0;
		virtual int DrawMesh(const std::string& shader, const std::string& mesh_name) =0;
		virtual int DeleteMesh(const std::string& mesh_name) = 0;

		protected:
		std::unordered_map<std::string, std::unique_ptr<Mesh>> mesh_map;
	};


	class VulkanMeshManager : public MeshManager
	{
	public:
		VulkanMeshManager(Vulkan_type* vulkan_type, VulkanShaderManager* shader_manager);
		VulkanMeshManager(const VulkanMeshManager& mesh_manager) = delete;
		int AddMesh(const std::string& mesh_name) override;
		int DrawMesh(const std::string& shader, const std::string& mesh_name) override;
		int DeleteMesh(const std::string& mesh_name) override;


	private:
		Vulkan_type* vulkan_type;
		VulkanShaderManager* shader_manager;
	};
}


#endif
