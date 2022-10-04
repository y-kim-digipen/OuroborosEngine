#include "vulkan_descriptor_set.h"

namespace Renderer {

    DescriptorSet& DescriptorSet::Init(VulkanType* vk_type, uint32_t set_num)
    {
        vulkan_type = vk_type;
        num = set_num;

        return *this;
    }

    DescriptorSet& DescriptorSet::AddBindingLayout(uint32_t binding_num, VkDescriptorType descriptor_type, VkShaderStageFlags stage_flags)
    {

        if (layout_bindings.size() <= binding_num) {
            layout_bindings.resize(binding_num + 1);
        }
        
        layout_bindings[binding_num].binding = binding_num;
        layout_bindings[binding_num].descriptorType = descriptor_type;
        layout_bindings[binding_num].descriptorCount = 1;
        layout_bindings[binding_num].stageFlags = stage_flags;


        return *this;
	}

    DescriptorSet& DescriptorSet::AddBinding(uint32_t binding_num, VulkanUniformBuffer* vulkan_ubo)
    {
        vkQueueWaitIdle(vulkan_type->device.graphics_queue);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            frame_data[i].buffers_info[binding_num] = vulkan_ubo->GetBufferInfo(i);
        }
        
		if (layout != VK_NULL_HANDLE) {
            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

                frame_data[i].buffers_info[binding_num] = vulkan_ubo->GetBufferInfo(i);

				VkWriteDescriptorSet set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                set_write.dstSet = frame_data[i].set;
                set_write.dstBinding = binding_num;
                set_write.descriptorCount = 1;
                set_write.descriptorType = layout_bindings[binding_num].descriptorType;
                set_write.pBufferInfo = &frame_data[i].buffers_info[binding_num];

				vkUpdateDescriptorSets(vulkan_type->device.handle, 1, &set_write, 0, nullptr);
            }
        }

        return *this;
    }

    DescriptorSet& DescriptorSet::AddBinding(uint32_t binding_num, VulkanTexture* vulkan_texture)
    {
        vkQueueWaitIdle(vulkan_type->device.graphics_queue);

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            images_info[binding_num] = (vulkan_texture)->GetImageInfo();
        }

		if (layout != VK_NULL_HANDLE) {
            images_info[binding_num] = (vulkan_texture)->GetImageInfo();

            for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

				VkWriteDescriptorSet set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                set_write.dstSet = frame_data[i].set;
                set_write.dstBinding = binding_num;
                set_write.descriptorCount = 1;
                set_write.descriptorType = layout_bindings[binding_num].descriptorType;
                set_write.pImageInfo = &images_info[binding_num];

				vkUpdateDescriptorSets(vulkan_type->device.handle, 1, &set_write, 0, nullptr);
            }


        }


        return *this;
    }

    void DescriptorSet::Build()
    {
        VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
        set_layout_create_info.bindingCount = layout_bindings.size();
        set_layout_create_info.pBindings = layout_bindings.data();
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &layout));

        VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
        alloc_info.descriptorPool = vulkan_type->descriptor_pool;
        alloc_info.descriptorSetCount = 1;
        alloc_info.pSetLayouts = &layout;

        uint32_t binding_count = layout_bindings.size();

        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &frame_data[i].set));

            std::vector<VkWriteDescriptorSet> set_writes;

            // first(key) = binding_num, second(value) = buffer_info
            for (const auto& buffer_info : frame_data[i].buffers_info) {

                VkWriteDescriptorSet set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                set_write.dstSet = frame_data[i].set;
                set_write.dstBinding = buffer_info.first;
                set_write.descriptorCount = 1;
                set_write.descriptorType = layout_bindings[buffer_info.first].descriptorType;
                set_write.pBufferInfo = &buffer_info.second;
                
                set_writes.push_back(set_write);
            }

            // first(key) = binding_num, second(value) = image_info
            for (const auto& image_info : images_info) {
                VkWriteDescriptorSet set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
                set_write.dstSet = frame_data[i].set;
                set_write.dstBinding = image_info.first;
                set_write.descriptorCount = 1;
                set_write.descriptorType = layout_bindings[image_info.first].descriptorType;
                set_write.pImageInfo = &image_info.second;
                set_writes.push_back(set_write);
            }

            vkUpdateDescriptorSets(vulkan_type->device.handle, binding_count, set_writes.data(), 0, nullptr);
        }
    }

    void DescriptorSet::Bind()
    {
        vkCmdBindDescriptorSets(vulkan_type->frame_data[vulkan_type->current_frame].command_buffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            vulkan_type->current_pipeline_layout,
            0,
            1,
            &frame_data[vulkan_type->current_frame].set,
            0,
            nullptr
        );
    }

    void DescriptorSet::Cleanup()
    {
        if (layout != VK_NULL_HANDLE) {
            vkDestroyDescriptorSetLayout(vulkan_type->device.handle, layout, nullptr);
            layout = VK_NULL_HANDLE;
        }
        
        for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            if (frame_data[i].set != VK_NULL_HANDLE) {
                VK_CHECK(vkFreeDescriptorSets(vulkan_type->device.handle, vulkan_type->descriptor_pool, 1, &frame_data[i].set));
                frame_data[i].set = VK_NULL_HANDLE;
            }
        }
    }

}
