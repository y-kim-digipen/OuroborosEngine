#include "vulkan_initializers.h"

namespace Renderer
{
	VkDescriptorSetLayoutBinding VulkanInitializer::DescripterSetLayoutBinding(VkDescriptorType type,
		VkShaderStageFlags stage_flags, uint32_t binding, uint32_t descriptor_count)
	{
		VkDescriptorSetLayoutBinding set_layout_binding{};
		set_layout_binding.descriptorType = type;
		set_layout_binding.stageFlags = stage_flags;
		set_layout_binding.binding = binding;
		set_layout_binding.descriptorCount = descriptor_count;
		return set_layout_binding;
	}

	VkDescriptorSetLayoutCreateInfo VulkanInitializer::DescriptorSetLayoutCreateInfo(
		const VkDescriptorSetLayoutBinding* p_bindings, uint32_t binding_count)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pBindings = p_bindings;
		descriptor_set_layout_create_info.bindingCount = binding_count;
		return descriptor_set_layout_create_info;
	}

	VkSemaphoreCreateInfo VulkanInitializer::SemaphoreCreateInfo()
	{
		VkSemaphoreCreateInfo semaphore_create_info{};
		semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		return semaphore_create_info;
	}

	VkCommandBufferBeginInfo VulkanInitializer::CommandBufferBeginInfo()
	{
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		return command_buffer_begin_info;
	}

	VkRenderPassBeginInfo VulkanInitializer::RenderPassBeginInfo()
	{
		VkRenderPassBeginInfo render_pass_begin_info{};
		render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		return render_pass_begin_info;
	}

	VkBool32 VulkanInitializer::GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat)
	{
		std::vector<VkFormat> depth_formats = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_FORMAT_D32_SFLOAT,
			VK_FORMAT_D24_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM_S8_UINT,
			VK_FORMAT_D16_UNORM
		};

		for (auto& format : depth_formats)
		{
			VkFormatProperties format_properties;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &format_properties);
			if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depthFormat = format;
				return true;
			}
		}

		return false;
	}

	VkSamplerCreateInfo VulkanInitializer::SamplerCreateInfo()
	{
		VkSamplerCreateInfo sampler_create_info{};
		sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_create_info.maxAnisotropy = 1.0f;
		return sampler_create_info;

	}

	VkDescriptorSetAllocateInfo VulkanInitializer::DescriptorSetAllocateInfo(VkDescriptorPool descriptor_pool,
		const VkDescriptorSetLayout* p_set_layouts, uint32_t descriptor_set_count)
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = descriptor_pool;
		descriptor_set_allocate_info.pSetLayouts = p_set_layouts;
		descriptor_set_allocate_info.descriptorSetCount = descriptor_set_count;
		return descriptor_set_allocate_info;
	}

	VkDescriptorImageInfo VulkanInitializer::DescriptorImageInfo(VkSampler sampler, VkImageView image_view,
		VkImageLayout image_layout)
	{
		VkDescriptorImageInfo descriptor_image_info{};
		descriptor_image_info.sampler = sampler;
		descriptor_image_info.imageView = image_view;
		descriptor_image_info.imageLayout = image_layout;
		return descriptor_image_info;
	}

	VkWriteDescriptorSet VulkanInitializer::WriteDescriptorSet(VkDescriptorSet dst_set, VkDescriptorType type,
		uint32_t binding, VkDescriptorBufferInfo* buffer_info, uint32_t descriptor_count)
	{

		VkWriteDescriptorSet write_descriptor_set{};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.dstSet = dst_set;
		write_descriptor_set.descriptorType = type;
		write_descriptor_set.dstBinding = binding;
		write_descriptor_set.pBufferInfo = buffer_info;
		write_descriptor_set.descriptorCount = descriptor_count;
		return write_descriptor_set;
	}

	VkWriteDescriptorSet VulkanInitializer::WriteDescriptorSet(VkDescriptorSet dst_set, VkDescriptorType type,
		uint32_t binding, VkDescriptorImageInfo* image_info, uint32_t descriptor_count)
	{
		VkWriteDescriptorSet write_descriptor_set{};
		write_descriptor_set.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set.dstSet = dst_set;
		write_descriptor_set.descriptorType = type;
		write_descriptor_set.dstBinding = binding;
		write_descriptor_set.pImageInfo = image_info;
		write_descriptor_set.descriptorCount = descriptor_count;
		return write_descriptor_set;
	}

	VkDescriptorSetLayoutCreateInfo VulkanInitializer::DescriptorSetLayoutCreateInfo(std::vector<VkDescriptorSetLayoutBinding>& bindings)
	{
		VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info{};
		descriptor_set_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		descriptor_set_layout_create_info.pBindings = bindings.data();
		descriptor_set_layout_create_info.bindingCount = static_cast<uint32_t>(bindings.size());
		return descriptor_set_layout_create_info;
	}

	VkPipelineLayoutCreateInfo VulkanInitializer::pipelineLayoutCreateInfo(const VkDescriptorSetLayout* p_set_layouts,uint32_t set_layout_count)
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info{};
		pipeline_layout_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipeline_layout_create_info.setLayoutCount = set_layout_count;
		pipeline_layout_create_info.pSetLayouts = p_set_layouts;
		return pipeline_layout_create_info;
	}
}
