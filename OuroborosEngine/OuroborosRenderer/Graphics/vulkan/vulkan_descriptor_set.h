#ifndef VULKAN_DESCRIPTOR_SET_H
#define VULKAN_DESCRIPTOR_SET_H

#include "vulkan_type.inl"

#include "vulkan_buffer.h"
#include "vulkan_texture.h"

#include <unordered_map>

namespace Renderer {
	struct DescriptorSet {

		VulkanType* vulkan_type;
		uint32_t num; //set_number

		struct FrameData {
			VkDescriptorSet set;
			std::unordered_map<uint32_t ,VkDescriptorBufferInfo> buffers_info;
		};

		FrameData frame_data[MAX_FRAMES_IN_FLIGHT];
		std::unordered_map<uint32_t ,VkDescriptorImageInfo> images_info; // texture info is read-only data, so dont need to be in frame data

		std::vector<VkDescriptorSetLayoutBinding> layout_bindings;
		VkDescriptorSetLayout layout;

		DescriptorSet& Init(VulkanType* vk_type, uint32_t set_num);

		DescriptorSet& AddBufferBinding(
			uint32_t binding_num,
			VkDescriptorType descriptor_type,
			VkShaderStageFlags stage_flags,
			VulkanUniformBuffer* vulkan_ubo
			);

		DescriptorSet& AddTextureBinding(
			uint32_t binding_num,
			VkDescriptorType descriptor_type,
			VkShaderStageFlags stage_flags,
			VulkanTexture* vulkan_texture
		);

		// build descriptor set layout & create descriptor sets
		void Build();

		// bind current_frame
		void Bind();
		void Cleanup();
	};
}

#endif // !VULKAN_DESCRIPTOR_SET_H
