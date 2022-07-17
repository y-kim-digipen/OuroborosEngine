#include "vulkan_initializers.h"

#include <vector>

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

	VkDescriptorImageInfo VulkanInitializer::DescriptorImageInfo(VkSampler sampler, VkImageView image_view,
		VkImageLayout image_layout)
	{
		VkDescriptorImageInfo descriptor_image_info{};
		descriptor_image_info.sampler = sampler;
		descriptor_image_info.imageView = image_view;
		descriptor_image_info.imageLayout = image_layout;
		return descriptor_image_info;
	}

	VkBool32 VulkanInitializer::GetSupportDepthFormat(VkPhysicalDevice physical_device, VkFormat* depth_format)
	{
		std::vector<VkFormat> depth_formats = 
		{
		VK_FORMAT_D32_SFLOAT_S8_UINT,
		VK_FORMAT_D32_SFLOAT,
		VK_FORMAT_D24_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM_S8_UINT,
		VK_FORMAT_D16_UNORM
		};

		for (auto& format : depth_formats)
		{
			VkFormatProperties format_properties;
			vkGetPhysicalDeviceFormatProperties(physical_device, format, &format_properties);
	
			if (format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
			{
				*depth_format = format;
				return true;
			}
		}

		return false;

	}

	VkSamplerCreateInfo VulkanInitializer::SamplerCreateInfo()
	{
		VkSamplerCreateInfo samplerCreateInfo{};
		samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerCreateInfo.maxAnisotropy = 1.0f;
		return samplerCreateInfo;
	}


	VkPipelineShaderStageCreateInfo VulkanInitializer::PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
	                                                                                 VkShaderModule shader_module)
	{
		VkPipelineShaderStageCreateInfo shader_stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
		shader_stage_create_info.stage = stage;
		shader_stage_create_info.module = shader_module;
		shader_stage_create_info.pName = "main";

		return shader_stage_create_info;
	}

	 VkPipelineVertexInputStateCreateInfo VulkanInitializer::PipelineVertexInputStageCreateInfo()
	{
		VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

		pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
		pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;

		return pipeline_vertex_input_state_create_info;
	}

	 VkPipelineInputAssemblyStateCreateInfo VulkanInitializer::PipelineInputAssemblyStateCreateInfo(
		VkPrimitiveTopology topology)
	{
		VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
		pipeline_input_assembly_state_create_info.topology = topology;
		pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

		return pipeline_input_assembly_state_create_info;

	}

	 VkPipelineRasterizationStateCreateInfo VulkanInitializer::PipelineRasterizationStateCreateInfo(
		VkPolygonMode polygon_mode, VkCullModeFlags cull_mode)
	{
		VkPipelineRasterizationStateCreateInfo pipeline_rasterization_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO };
		pipeline_rasterization_state_create_info.depthClampEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.rasterizerDiscardEnable = VK_FALSE;

		pipeline_rasterization_state_create_info.polygonMode = polygon_mode;
		pipeline_rasterization_state_create_info.lineWidth = 1.0f;
		pipeline_rasterization_state_create_info.cullMode = VK_CULL_MODE_BACK_BIT;
		pipeline_rasterization_state_create_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

		pipeline_rasterization_state_create_info.depthBiasEnable = VK_FALSE;
		pipeline_rasterization_state_create_info.depthBiasConstantFactor = 0.f;
		pipeline_rasterization_state_create_info.depthBiasClamp = 0.f;
		pipeline_rasterization_state_create_info.depthBiasSlopeFactor = 0.f;

		return pipeline_rasterization_state_create_info;
	}

	 VkPipelineMultisampleStateCreateInfo VulkanInitializer::PipelineMultisampleStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

		pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
		pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		pipeline_multisample_state_create_info.minSampleShading = 1.0f;
		pipeline_multisample_state_create_info.pSampleMask = nullptr;
		pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
		pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;
		pipeline_multisample_state_create_info.flags = 0;

		return pipeline_multisample_state_create_info;
	}

	 VkPipelineColorBlendAttachmentState VulkanInitializer::PipelineColorBlendAttachmentState(VkColorComponentFlags color_write_mask,
		 VkBool32 blend_enable)
	{
		VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
		pipeline_color_blend_attachment_state.colorWriteMask = color_write_mask;
		pipeline_color_blend_attachment_state.blendEnable = blend_enable;
		return pipeline_color_blend_attachment_state;
	}

	 VkPipelineLayoutCreateInfo VulkanInitializer::PipelineLayoutCreateInfo(VkDescriptorSetLayout* p_set_layouts, uint32_t layout_count, VkPushConstantRange* p_constant_ranges, uint32_t constant_count)
	{
		VkPipelineLayoutCreateInfo pipeline_layout_create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

		pipeline_layout_create_info.flags = 0;
		pipeline_layout_create_info.setLayoutCount = layout_count;
		pipeline_layout_create_info.pSetLayouts = p_set_layouts;
		pipeline_layout_create_info.pushConstantRangeCount = constant_count;
		pipeline_layout_create_info.pPushConstantRanges = p_constant_ranges;


		return pipeline_layout_create_info;
	}

	 VkPipelineDepthStencilStateCreateInfo VulkanInitializer::DepthStencilCreateInfo(bool b_depth_test,
		bool b_depth_write, VkCompareOp compare_op)
	{
		VkPipelineDepthStencilStateCreateInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		info.pNext = nullptr;

		info.depthTestEnable = b_depth_test ? VK_TRUE : VK_FALSE;
		info.depthWriteEnable = b_depth_write ? VK_TRUE : VK_FALSE;
		info.depthCompareOp = b_depth_test ? compare_op : VK_COMPARE_OP_ALWAYS;
		info.depthBoundsTestEnable = VK_FALSE;
		info.minDepthBounds = 0.0f; // Optional
		info.maxDepthBounds = 1.0f; // Optional
		info.stencilTestEnable = VK_FALSE;

		return info;
	}

	 VkPipelineVertexInputStateCreateInfo VulkanInitializer::PipelineVertexInputStateCreateInfo()
	 {
		 VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
		 pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		 return pipelineVertexInputStateCreateInfo;
	 }

	 VkSemaphoreCreateInfo VulkanInitializer::SemaphoreCreateInfo()
	 {
		 VkSemaphoreCreateInfo semaphore_create_info{};
		 semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		 return semaphore_create_info;
	 }

	 VkRenderPassBeginInfo VulkanInitializer::RenderPassBeginInfo()
	 {
		 VkRenderPassBeginInfo render_pass_begin_info{};
		 render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		 return render_pass_begin_info;
	 }

	 VkCommandBufferBeginInfo VulkanInitializer::CommandBufferBeginInfo()
	 {
		 VkCommandBufferBeginInfo cmd_buffer_begin_info{};
		 cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		 return cmd_buffer_begin_info;
	 }
}
