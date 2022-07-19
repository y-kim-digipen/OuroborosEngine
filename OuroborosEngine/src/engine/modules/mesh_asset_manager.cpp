#include "mesh_asset_manager.h"

#ifndef TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_IMPLEMENTATION
#endif
#include <tinyobjloader/tiny_obj_loader.h>

#include "../common/assets.h"
#include "Graphics/mesh_manager.h"
#include "../engine.h"
#include "Graphics/vulkan/vulkan_context.h"

namespace OE
{
	int MeshAssetManager::LoadAsset(const std::string& file_path)
    {
        Asset::Mesh mesh;

        //std::string file_path = "model/";
        //file_path.append(file_name);

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str()))
        {
            //throw std::runtime_error(warn + err);
            std::cerr << "Failed to load asset " + file_path << std::endl;
            if (!warn.empty())
            {

                std::cerr << warn << std::endl;
            }
            if (!err.empty())
            {

                std::cerr << err << std::endl;
            }
            return 1;
        }

        std::unordered_map<Asset::Mesh::Vertex, uint32_t> uniqueVertices{};

        for (const auto& shape : shapes)
        {
            for (const auto& index : shape.mesh.indices)
            {
                Asset::Mesh::Vertex vertex{};

                vertex.position = {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                if (!attrib.texcoords.empty())
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                if (!attrib.normals.empty())
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2],
                };



                //vertex.color = { 1.0f, 1.0f, 1.0f };

                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
                    mesh.vertices.push_back(vertex);
                }
                mesh.indices.push_back(uniqueVertices[vertex]);
            }
        }
        assets[file_path] = std::make_pair(true, mesh);
        auto& vulkan_mesh_manager
		                        = static_cast<Renderer::VulkanContext*>
		                        (Engine().Get().window->GetWindowData().RenderContextData.get())
		                        ->mesh_manager_;
        vulkan_mesh_manager.CopyAssetData(file_path.c_str(), mesh);
        return 0;
    }

	int MeshAssetManager::UnloadAsset(const std::string& name)
	{
        int res =  AssetManager<Asset::Mesh>::UnloadAsset(name);
        if(!res)
        {
            return res;
        }
        auto& vulkan_mesh_manager
            = static_cast<Renderer::VulkanContext*>
            (Engine().Get().window.get()->GetWindowData().RenderContextData.get())
            ->mesh_manager_;
        vulkan_mesh_manager.DeleteMeshData(name);
        return 0;
	}

	void MeshAssetManager::CleanUp()
	{
        for(const auto& key : assets | std::views::keys)
        {
            auto& vulkan_mesh_manager
                = static_cast<Renderer::VulkanContext*>
                (Engine().Get().window.get()->GetWindowData().RenderContextData.get())
                ->mesh_manager_;
            vulkan_mesh_manager.DeleteMeshData(key);
        }
		AssetManager<Asset::Mesh>::CleanUp();
	}
}
