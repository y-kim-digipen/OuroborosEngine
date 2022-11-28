#include "vulkan_mesh.h"

#include "vulkan_type.inl"
#include "vulkan_buffer.h"

namespace Renderer {

    struct ModelConstant {
      glm::mat4 model;
      glm::mat4 normal_matrix;
    };

    VulkanMesh::VulkanMesh(VulkanType* vulkan_type) : vulkan_type(vulkan_type)
    {
    }
    VulkanMesh::~VulkanMesh()
    { 
    }

    bool VulkanMesh::CopyAssetData(const Asset::Mesh& mesh)
    {
        if (!Mesh::CopyAssetData(mesh))
            return false;

        p_vertex_buffer = std::make_unique<VulkanVertexBuffer>(vulkan_type,vertices);
        p_index_buffer = std::make_unique<VulkanIndexBuffer>(vulkan_type,indices);

        return true;
    }

    void VulkanMesh::Draw(const glm::mat4& model, const glm::mat4& normal_matrix)
    {
        VkCommandBuffer& command_buffer = vulkan_type->frame_data[vulkan_type->current_frame].command_buffer;
        //VkCommandBuffer& command_buffer = vulkan_type->deferred_pass.off_screen_command_buffer;

        ModelConstant model_constant{ model, normal_matrix };
        size_t pushconstant_size = sizeof(model_constant);
        
        vkCmdPushConstants(command_buffer, vulkan_type->current_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model_constant), &model_constant);

        p_index_buffer->Bind();
        p_vertex_buffer->Bind();

        vkCmdDrawIndexed(command_buffer, indices.size(), 1, 0, 0, 0);
    }
}
