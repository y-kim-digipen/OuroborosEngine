#pragma once
#include <vector>
#include <vulkan_core.h>

namespace Renderer
{
	struct Vulkan_PipelineBuilder
	{
        VkPipeline BuildPipeLine(VkDevice device, VkRenderPass render_pass);

        std::vector<VkPipelineShaderStageCreateInfo> shader_stages;
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
