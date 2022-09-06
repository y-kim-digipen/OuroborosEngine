#include "mesh_manager.h"

#include <iostream>

#include "vulkan/vulkan_mesh.h"
#include "mesh.h"
#include "shader_manager.h"

namespace Renderer
{
	VulkanMeshManager::VulkanMeshManager(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	int VulkanMeshManager::CopyAssetData(const std::string&& mesh_name, const Asset::Mesh& mesh)
	{
		if (mesh_map.find(mesh_name) != mesh_map.end()) 
		{
			std::cout << mesh_name << " already exists\n";
			return -1;
		}

		mesh_map[mesh_name] = std::make_unique<VulkanMesh>(vulkan_type);
		if (!mesh_map[mesh_name]->CopyAssetData(mesh)) 
		{
			std::cout << mesh_name << " loading failed\n";
			return -1;
		}

		return 0;
	}


	int VulkanMeshManager::DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat3& normal_matrix)
	{
		if (auto mesh_iter = mesh_map.find(mesh_name); mesh_iter != mesh_map.end())
		{
			mesh_iter->second->Draw(model, normal_matrix);
		}
		else
		{
			//std::cout << mesh_name << " (mesh) is doesn't exist! try to added mesh" << std::endl;
			throw std::runtime_error(mesh_name + "(mesh) is doesn't exist!");
			//int result = AddMesh(mesh_name);
			//if (result == 0)
			//{
			//	mesh_map.find(mesh_name)->second->Draw(model, normal_matrix);
			//}
		}

		return 0;
	}

	int VulkanMeshManager::DeleteMeshData(const std::string& mesh_name)
	{
		if(auto iter = mesh_map.find(mesh_name); iter != mesh_map.end())
		{
			iter->second.reset();
			//TODO(Austyn): erase vulkan_mesh
			//mesh_map[mesh_name]
			mesh_map.erase(iter);
			return 0;
		}
		else
		{
			std::cout << mesh_name << "doesn't exist!" << std::endl;
		}

		return 0;
	}

	void VulkanMeshManager::Cleanup()
	{


	}
}
