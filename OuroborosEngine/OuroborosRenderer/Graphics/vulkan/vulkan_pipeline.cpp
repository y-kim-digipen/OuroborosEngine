#include "vulkan_pipeline.h"

#include <iostream>

namespace Renderer
{
	VkPipeline Vulkan_PipelineBuilder::BuildPipeLine(VkDevice device, VkRenderPass render_pass)
	{
        VkPipelineViewportStateCreateInfo viewport_state{ VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO };
        viewport_state.viewportCount     = 1;
        viewport_state.pViewports        = &viewport;
        viewport_state.scissorCount      = 1;
        viewport_state.pScissors         = &scissor;

        VkPipelineColorBlendStateCreateInfo color_blend_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO };
        color_blend_state_create_info.logicOpEnable     = VK_FALSE;
        color_blend_state_create_info.logicOp           = VK_LOGIC_OP_COPY;
        color_blend_state_create_info.attachmentCount   = 1;
        color_blend_state_create_info.pAttachments      = &color_blend_attachment;

        VkGraphicsPipelineCreateInfo pipeline_create_info{ VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO };
        pipeline_create_info.stageCount           = static_cast<uint32_t>(shader_stages.size());
        pipeline_create_info.pStages              = shader_stages.data();
        pipeline_create_info.pVertexInputState    = &vertex_input_info;
        pipeline_create_info.pInputAssemblyState  = &input_assembly;
        pipeline_create_info.pViewportState       = &viewport_state;
        pipeline_create_info.pRasterizationState  = &rasterizer;
        pipeline_create_info.pMultisampleState    = &multisampling;
        pipeline_create_info.pColorBlendState     = &color_blend_state_create_info;
        pipeline_create_info.layout               = pipeline_layout;
        pipeline_create_info.renderPass           = render_pass;
        pipeline_create_info.subpass              = 0;
        pipeline_create_info.basePipelineHandle   = VK_NULL_HANDLE;
        pipeline_create_info.pDepthStencilState   = &depth_stencil;

        VkPipeline new_pipeline;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipeline_create_info, nullptr, &new_pipeline) != VK_SUCCESS) 
        {
            std::cout << "Failed to create pipeline\n";
            return VK_NULL_HANDLE;
        }

        return new_pipeline;

	}
}
