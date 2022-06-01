#ifndef MESHMANAGER_H
#define MESHMANAGER_H
#include <memory>
#include <unordered_map>

struct Vulkan_type;

namespace Renderer
{
	class ShaderManager;
	class Mesh;

	class MeshManager
	{
	public:
		MeshManager(Vulkan_type* vulkan_type, ShaderManager* shader_manager);
		MeshManager(const MeshManager& mesh_manager) = delete;
		int AddMesh(const char* mesh_name);
		int DrawMesh(const char* shader, const char* mesh_name);
		int DeleteMesh(const char* mesh_name);

		friend class ShaderManager;
	private:
		std::unordered_map<const char*, std::unique_ptr<Mesh>> mesh_map;
		Vulkan_type* vulkan_type;
		ShaderManager* shader_manager;
	};
}


#endif
