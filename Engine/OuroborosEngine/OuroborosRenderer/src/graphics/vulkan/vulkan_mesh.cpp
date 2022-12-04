#include "vulkan_mesh.h"

#include "vulkan_type.inl"
#include "vulkan_buffer.h"

namespace Renderer {

    struct ModelConstant {
      glm::mat4 model;
      glm::mat4 normal_matrix;
    };

    VulkanMesh::VulkanMesh(VulkanType* vulkan_type, Model* parent_model, uint64_t vertex_offset, uint64_t index_offset, uint64_t index_count)
        : vulkan_type(vulkan_type), parent_model(parent_model), vertex_offset(vertex_offset), index_offset(index_offset), index_count(index_count)
    {
    }
    VulkanMesh::~VulkanMesh()
    { 
    }

    void VulkanMesh::Draw(const glm::mat4& model, const glm::mat4& normal_matrix)
    {
        VkCommandBuffer& command_buffer = vulkan_type->frame_data[vulkan_type->current_frame].command_buffer;
        //VkCommandBuffer& command_buffer = vulkan_type->deferred_pass.off_screen_command_buffer;

        ModelConstant model_constant{ model, normal_matrix };
        size_t pushconstant_size = sizeof(model_constant);
        
        vkCmdPushConstants(command_buffer, vulkan_type->current_pipeline_layout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(model_constant), &model_constant);

        BindModel(parent_model);

        vkCmdDrawIndexed(command_buffer, index_count, 1, index_offset, 0, 0);
    }
}
