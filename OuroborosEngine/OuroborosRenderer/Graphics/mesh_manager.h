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
		virtual int AddMesh(const char* mesh_name) = 0;
		virtual int DrawMesh(const char* shader, const char* mesh_name) =0;
		virtual int DeleteMesh(const char* mesh_name) = 0;

		protected:
		std::unordered_map<const char*, std::unique_ptr<Mesh>> mesh_map;
	};


	class VulkanMeshManager : public MeshManager
	{
	public:
		VulkanMeshManager(Vulkan_type* vulkan_type, VulkanShaderManager* shader_manager);
		VulkanMeshManager(const VulkanMeshManager& mesh_manager) = delete;
		int AddMesh(const char* mesh_name);
		int DrawMesh(const char* shader, const char* mesh_name);
		int DeleteMesh(const char* mesh_name);


	private:
		Vulkan_type* vulkan_type;
		VulkanShaderManager* shader_manager;
	};
}


#endif
