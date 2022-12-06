#include "mesh_manager.h"

#include <iostream>
#include <memory>

#include "vulkan/vulkan_mesh.h"
#include "mesh.h"
#include "shader_manager.h"

namespace Renderer
{
	VulkanMeshManager::VulkanMeshManager(VulkanType* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	int VulkanMeshManager::CopyAssetData(const std::string&& model_name, const Asset::Mesh& mesh)
	{
		if (model_list.find(model_name) != model_list.end()) 
		{
			std::cout << model_name << " already exists\n";
			return -1;
		}

		// INSERT NEW MODEL
		model_list[model_name] = (new Model());
		Model* model = model_list[model_name];

		if (InitModel(vulkan_type, model, mesh) == false) {
			std::cout << model_name << " loading failed\n";
			return -1;
		}

        uint32_t submesh_count = mesh.payload_datas.size();

        for (const auto& mesh_payload : mesh.payload_datas) {
            model->mesh_list.push_back(
                std::make_unique<VulkanMesh>(
                    vulkan_type,
                    model,
                    mesh_payload.second.vertex_offset,
                    mesh_payload.second.index_offset,
                    mesh_payload.second.index_using_count
                    )
            );

			// INSERT SUBMESH TO MESH_MAP
			mesh_map[mesh_payload.first] = model->mesh_list.back().get();
        }
		
		return 0;
	}


	int VulkanMeshManager::DrawMesh(const std::string& mesh_name, const glm::mat4& model, const glm::mat4& normal_matrix)
	{
		if (auto mesh_iter = mesh_map.find(mesh_name); mesh_iter != mesh_map.end())
		{
			mesh_iter->second->Draw(model, normal_matrix);
		}
		else
		{
			std::cout << mesh_name << " doesn't exist!" << std::endl;
			return -1;
		}

		return 0;
	}

	int VulkanMeshManager::DeleteMeshData(const std::string& model_name)
	{
		if(auto iter = model_list.find(model_name); iter != model_list.end())
		{
			CleanupModel(iter->second);
			delete iter->second;
			iter->second = nullptr;
			model_list.erase(iter);
			return 0;
		}
		else
		{
			std::cout << model_name << "doesn't exist!" << std::endl;
		}

		return 0;
	}

	void VulkanMeshManager::Cleanup()
	{
		for (auto& model : model_list) {

			CleanupModel(model.second);
			delete model.second;
			model.second = nullptr;
		}
	}
}
