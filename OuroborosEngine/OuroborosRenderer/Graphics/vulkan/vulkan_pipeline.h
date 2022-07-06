#pragma once

#include "vulkan_type.inl"

#include <vector>


namespace Renderer
{
	struct DeferredPipeline
	{
		
		 
	};


	struct Vulkan_PipelineBuilder
	{
		VkPipeline BuildPipeLine(VkDevice device, VkRenderPass render_pass, std::vector<VkPipelineShaderStageCreateInfo>& shader_stage_create_infos);
		VkPipelineLayout BuildPipeLineLayout(VkDevice device, VkDescriptorSetLayout* p_set_layouts, uint32_t layout_count, VkPushConstantRange* p_constant_ranges, uint32_t constant_count);

		VkPipelineVertexInputStateCreateInfo vertex_input_info;
		VkPipelineInputAssemblyStateCreateInfo input_assembly;
		VkViewport viewport;
		VkRect2D scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer;
		VkPipelineColorBlendAttachmentState color_blend_attachment;
		VkPipelineMultisampleStateCreateInfo multisampling;
		VkPipelineLayout pipeline_layout;
		VkPipelineDepthStencilStateCreateInfo depth_stencil;
	};

	

}
