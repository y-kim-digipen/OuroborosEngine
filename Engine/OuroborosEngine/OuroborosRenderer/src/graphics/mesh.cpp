#include "mesh.h"

#include "vulkan_type.inl"
#include "vulkan_buffer.h"
#include "vulkan_mesh.h"

namespace Renderer {

    bool InitModel(VulkanType* vulkan_type, Model* out_model, const Asset::Mesh& mesh)
    {
		if (out_model == nullptr /*|| vulkan_type == nullptr*/)
            return false;

        for (const auto& element : mesh.vertices)
        {
	        Vertex vertex;
	        vertex.pos = element.position;
            vertex.normal = element.normal;
            vertex.uv = element.uv;
            vertex.tangent = element.tangent;
            out_model->vertices.push_back(vertex);
        }
		out_model->indices = mesh.indices;
        out_model->p_vertex_buffer = std::make_unique<VulkanVertexBuffer>(vulkan_type, out_model->vertices);
        out_model->p_index_buffer = std::make_unique<VulkanIndexBuffer>(vulkan_type, out_model->indices);

        return true;
    }
    bool BindModel(Model* model)
    {
        if (model == nullptr)
            return false;

        model->p_index_buffer->Bind();
        model->p_vertex_buffer->Bind();

        return true;
    }
    void CleanupModel(Model* model)
    {
        if (model->p_index_buffer != nullptr) {
            //model->p_index_buffer->Cleanup();
            model->p_index_buffer.reset();
        }

		if (model->p_vertex_buffer != nullptr) {
            //model->p_vertex_buffer->Cleanup();
            model->p_vertex_buffer.reset();
        }

        model->mesh_list.clear();
    }
}
