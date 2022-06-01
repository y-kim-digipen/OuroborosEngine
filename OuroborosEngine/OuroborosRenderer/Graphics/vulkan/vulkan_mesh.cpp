#include "vulkan_mesh.h"

#include "vulkan_type.inl"
#include "vulkan_buffer.h"

namespace Renderer {
    VulkanMesh::VulkanMesh(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
    {
    }
    VulkanMesh::~VulkanMesh()
    {
    }

    bool VulkanMesh::LoadAsset(const char* file_name)
    {
        if (!Mesh::LoadAsset(file_name))
            return false;

        p_vertex_buffer = std::make_unique<VulkanVertexBuffer>(vulkan_type,vertices);
        p_index_buffer = std::make_unique<VulkanIndexBuffer>(vulkan_type,indices);

        return true;
    }

    void VulkanMesh::Draw()
    {
        VkCommandBuffer& command_buffer = vulkan_type->frame_data[vulkan_type->current_frame].command_buffer;

        p_index_buffer->Bind();
        p_vertex_buffer->Bind();


 
        vkCmdDrawIndexed(command_buffer, indices.size(), 1, 0, 0, 0);
    }

}
