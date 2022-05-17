#pragma once

#include <vulkan.h>
//#include <vulkan_win32.h>

#include <vector>
#include <cassert>
#include <optional>

#define VK_CHECK(call) \
	do {	\
		VkResult result_ = call; \
		assert(result_ == VK_SUCCESS); \
	} while(0)


struct VulkanDevice
{
	VkPhysicalDevice physical_device;
	VkDevice handle;
	VkQueue graphics_queue;
	VkQueue present_queue;
};

struct SwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

struct VulkanSwapChain
{

	SwapChainSupportDetails detail;

	VkFormat swap_chain_image_format;
	VkExtent2D swap_chain_extent;
	VkSwapchainKHR handle;
	std::vector<VkImage> swap_chain_images;
	std::vector<VkImageView> swap_chain_image_views;
	std::vector<VkFramebuffer> swap_chain_framebuffers;
};



struct QueueFamilyIndices
{
	std::optional<uint32_t> graphics_family;
	std::optional<uint32_t> present_family;
	bool isComplete()
	{
		return graphics_family.has_value() && present_family.has_value();
	}
};
struct VulkanCommand
{
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
};

struct Vulkan_type
{

#if defined(_DEBUG)
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
	
	VkInstance instance;
	VulkanDevice device;
	VkSurfaceKHR surface;
	VulkanSwapChain swap_chain;
	VkRenderPass render_pass;
	VulkanCommand command;
	VkPipeline graphic_pipeline;
};

struct VulkanInitializer
{
	static VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader_module);
	static VkPipelineVertexInputStateCreateInfo PipelineVertexInputStageCreateInfo();
	static VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology);
	static VkPipelineRasterizationStateCreateInfo  PipelineRasterizationStateCreateInfo(VkPolygonMode polygon_mode);
	static VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo();
	static VkPipelineColorBlendAttachmentState  PipelineColorBlendAttachmentState();
	static VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo();
	static VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool b_depth_test, bool b_depth_write, VkCompareOp compare_op);

};

inline VkPipelineShaderStageCreateInfo VulkanInitializer::PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage,
	VkShaderModule shader_module)
{
	VkPipelineShaderStageCreateInfo shader_stage_create_info{ VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO };
	shader_stage_create_info.stage = stage;
	shader_stage_create_info.module = shader_module;
	shader_stage_create_info.pName = "main";

	return shader_stage_create_info;
}

inline VkPipelineVertexInputStateCreateInfo VulkanInitializer::PipelineVertexInputStageCreateInfo()
{
	VkPipelineVertexInputStateCreateInfo pipeline_vertex_input_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO };

	pipeline_vertex_input_state_create_info.vertexBindingDescriptionCount = 0;
	pipeline_vertex_input_state_create_info.vertexAttributeDescriptionCount = 0;

	return pipeline_vertex_input_state_create_info;
}

inline VkPipelineInputAssemblyStateCreateInfo VulkanInitializer::PipelineInputAssemblyStateCreateInfo(
	VkPrimitiveTopology topology)
{
	VkPipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO };
	pipeline_input_assembly_state_create_info.topology = topology;
	pipeline_input_assembly_state_create_info.primitiveRestartEnable = VK_FALSE;

	return pipeline_input_assembly_state_create_info;

}

inline VkPipelineRasterizationStateCreateInfo VulkanInitializer::PipelineRasterizationStateCreateInfo(
	VkPolygonMode polygon_mode)
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

inline VkPipelineMultisampleStateCreateInfo VulkanInitializer::PipelineMultisampleStateCreateInfo()
{
	VkPipelineMultisampleStateCreateInfo pipeline_multisample_state_create_info{ VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO };

	pipeline_multisample_state_create_info.sampleShadingEnable = VK_FALSE;
	pipeline_multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	pipeline_multisample_state_create_info.minSampleShading = 1.0f;
	pipeline_multisample_state_create_info.pSampleMask = nullptr;
	pipeline_multisample_state_create_info.alphaToCoverageEnable = VK_FALSE;
	pipeline_multisample_state_create_info.alphaToOneEnable = VK_FALSE;

	return pipeline_multisample_state_create_info;
}

inline VkPipelineColorBlendAttachmentState VulkanInitializer::PipelineColorBlendAttachmentState()
{
	VkPipelineColorBlendAttachmentState pipeline_color_blend_attachment_state{};
	pipeline_color_blend_attachment_state.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	pipeline_color_blend_attachment_state.blendEnable = VK_FALSE;
	return pipeline_color_blend_attachment_state;
}

inline VkPipelineLayoutCreateInfo VulkanInitializer::PipelineLayoutCreateInfo()
{
	VkPipelineLayoutCreateInfo pipeline_layout_create_info{ VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO };

	pipeline_layout_create_info.flags = 0;
	pipeline_layout_create_info.setLayoutCount = 0;
	pipeline_layout_create_info.pSetLayouts = nullptr;
	pipeline_layout_create_info.pushConstantRangeCount = 0;
	pipeline_layout_create_info.pPushConstantRanges = nullptr;

	return pipeline_layout_create_info;
}

inline VkPipelineDepthStencilStateCreateInfo VulkanInitializer::DepthStencilCreateInfo(bool b_depth_test,
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

namespace Renderer {
	static Vulkan_type vulkan_type;
}
