#include "mesh_manager.h"

#include <iostream>

#include "vulkan/vulkan_mesh.h"
#include "mesh.h"
#include "shader_manager.h"

namespace Renderer
{
	MeshManager::MeshManager(Vulkan_type* vulkan_type, ShaderManager* shader_manager) : vulkan_type(vulkan_type), shader_manager(shader_manager)
	{

	}
	int MeshManager::AddMesh(const char* mesh_name)
	{
		if (mesh_map.find(mesh_name) != mesh_map.end()) 
		{
			std::cout << mesh_name << " already exists\n";
			return -1;
		}

		mesh_map[mesh_name] = std::make_unique<VulkanMesh>(vulkan_type);
		if (!mesh_map[mesh_name]->LoadAsset(mesh_name)) 
		{
			std::cout << mesh_name << " loading failed\n";
			return -1;
		}

		return 0;
	}


	int MeshManager::DrawMesh(const char* shader, const char* mesh_name)
	{
		if(auto iter = shader_manager->shader_map.find(shader); iter != shader_manager->shader_map.end())
		{
			iter->second->Bind();
			if(auto mesh_iter = mesh_map.find(mesh_name); mesh_iter != mesh_map.end())
			{
				mesh_iter->second->Draw();
			}
			else
			{
				std::cout << mesh_name << " (mesh) is doesn't exist! try to added mesh" << std::endl;
				int result = AddMesh(mesh_name);
				if(result == 0)
				{
					mesh_iter->second->Draw();
				}
			}
		}
		else
		{
			std::cout << shader << " (shader) is doesn't exist!" << std::endl;
			return -1;
		}

		return 0;
	}

	int MeshManager::DeleteMesh(const char* mesh_name)
	{
		if(auto iter = mesh_map.find(mesh_name); iter != mesh_map.end())
		{
			iter->second.reset();
			mesh_map.erase(iter);
			return 0;
		}
		else
		{
			std::cout << mesh_name << "doesn't exist!" << std::endl;
		}

		return 0;
	}
}
