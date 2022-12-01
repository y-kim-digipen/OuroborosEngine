#include "mesh_asset_manager.h"

//#ifndef TINYOBJLOADER_IMPLEMENTATION
//#define TINYOBJLOADER_IMPLEMENTATION
//#endif
//#include <tiny_obj_loader.h>

#include <common/assets.h>
#include <mesh_manager.h>
#include <vulkan_context.h>
#include "../core/engine.h"

#include "ModelLoader.h"

namespace OE
{
	int MeshAssetManager::LoadAsset(const std::string& file_path)
    {
        Asset::Mesh mesh;

        //std::string file_path = "model/";
        //file_path.append(file_name);



        yk_ModelLoader::ModelLoadResult result;
        yk_ModelLoader::ModelLoader::Import(file_path.c_str(), &result);
        auto& vulkan_mesh_manager
                              = static_cast<Renderer::VulkanContext*>
                              (Engine().Get().window->GetWindowData().RenderContextData.get())
                              ->mesh_manager;

        std::vector<Asset::Mesh::Vertex> vertices;
        const uint64_t num_vertices = result._numVertices;
        vertices.resize(num_vertices);

        for(uint64_t i = 0; i < num_vertices; ++i)
        {
            Asset::Mesh::Vertex v;
            v.position = result._mesh->_vertexPositions[i];
            v.normal = result._mesh->_vertexNormals[i];
            v.uv = result._mesh->_vertexTexCoords[i];
            vertices[i] = v;
        }
        mesh.indices = result._mesh->_indices;
        mesh.vertices = vertices;

        const uint32_t num_entry_points = result._mesh->_meshEntryPoints.size();
        for(const auto& entry_point : result._mesh->_meshEntryPoints)
        {
            std::string name = file_path + '|' + entry_point._name;
            if(num_entry_points == 1)
            {
                name = file_path + '|' + "whole";
            }
            Asset::MeshPayload payload;
            payload.vertex_offset = entry_point._vertexOffset;
            payload.vertex_using_count = entry_point._numVertices;
            payload.index_offset = entry_point._indexOffset;
            payload.index_using_count = entry_point._numIndices;
            mesh.payload_datas[name] = payload;
            //;
        }
        if(num_entry_points > 1)
        {
            Asset::MeshPayload payload;
            std::string name = file_path + '|' + "whole";
            payload.index_offset = 0;
            payload.index_using_count = result._numIndices;
            payload.vertex_offset = 0;
            payload.vertex_using_count = result._numVertices;
            mesh.payload_datas[name] = payload;
        }
        
        //mesh
        //assets[file_path] = std::make_pair(true, mesh);

        for(const auto& payload : mesh.payload_datas)
        {
	        assets[payload.first] = std::make_pair(true, mesh);
        }

        vulkan_mesh_manager->CopyAssetData(file_path.c_str(), mesh);
        //tinyobj::attrib_t attrib;
        //std::vector<tinyobj::shape_t> shapes;
        //std::vector<tinyobj::material_t> materials;
        //std::string warn, err;
        //if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, file_path.c_str()))
        //{
        //    //throw std::runtime_error(warn + err);
        //    std::cerr << "Failed to load asset " + file_path << std::endl;
        //    if (!warn.empty())
        //    {

        //        std::cerr << warn << std::endl;
        //    }
        //    if (!err.empty())
        //    {

        //        std::cerr << err << std::endl;
        //    }
        //    return 1;
        //}

        //std::unordered_map<Asset::Mesh::Vertex, uint32_t> uniqueVertices{};

        //for (const auto& shape : shapes)
        //{
        //    for (const auto& index : shape.mesh.indices)
        //    {
        //        Asset::Mesh::Vertex vertex{};

        //        vertex.position = {
        //            attrib.vertices[3 * index.vertex_index + 0],
        //            attrib.vertices[3 * index.vertex_index + 1],
        //            attrib.vertices[3 * index.vertex_index + 2]
        //        };

        //        if (!attrib.texcoords.empty())
        //            vertex.uv = {
        //                attrib.texcoords[2 * index.texcoord_index + 0],
        //                1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
        //        };

        //        if (!attrib.normals.empty())
        //            vertex.normal = {
        //                attrib.normals[3 * index.normal_index + 0],
        //                attrib.normals[3 * index.normal_index + 1],
        //                attrib.normals[3 * index.normal_index + 2],
        //        };



        //        //vertex.color = { 1.0f, 1.0f, 1.0f };

        //        if (uniqueVertices.count(vertex) == 0) {
        //            uniqueVertices[vertex] = static_cast<uint32_t>(mesh.vertices.size());
        //            mesh.vertices.push_back(vertex);
        //        }
        //        mesh.indices.push_back(uniqueVertices[vertex]);
        //    }
        //}
        //assets[file_path] = std::make_pair(true, mesh);
        //auto& vulkan_mesh_manager
		      //                  = static_cast<Renderer::VulkanContext*>
		      //                  (Engine().Get().window->GetWindowData().RenderContextData.get())
		      //                  ->mesh_manager;
        //vulkan_mesh_manager->CopyAssetData(file_path.c_str(), mesh);
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
            ->mesh_manager;
        vulkan_mesh_manager->DeleteMeshData(name);
        return 0;
	}

	void MeshAssetManager::CleanUp()
	{
        for(const auto& key : assets | std::views::keys)
        {
            auto& vulkan_mesh_manager
                = static_cast<Renderer::VulkanContext*>
                (Engine().Get().window.get()->GetWindowData().RenderContextData.get())
                ->mesh_manager;
            vulkan_mesh_manager->DeleteMeshData(key);
        }
		AssetManager<Asset::Mesh>::CleanUp();
	}
}
