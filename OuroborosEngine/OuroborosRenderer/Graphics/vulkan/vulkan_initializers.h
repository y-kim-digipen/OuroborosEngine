#ifndef VULKAN_INITIALIZER_H
#define VULKAN_INITIALIZER_H
#include <vulkan_core.h>


namespace Renderer
{

	class VulkanInitializer
	{
	public:
		static VkDescriptorSetLayoutBinding DescripterSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding, uint32_t descriptor_count =1);
		static VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* p_bindings, uint32_t binding_count);

		static VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dst_set,VkDescriptorType type,uint32_t binding,VkDescriptorBufferInfo* buffer_info,uint32_t descriptor_count = 1);
		static VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dst_set,VkDescriptorType type,uint32_t binding,VkDescriptorImageInfo* image_info,uint32_t descriptor_Count = 1);
		static VkDescriptorImageInfo DescriptorImageInfo(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);


		static VkBool32 GetSupportDepthFormat(VkPhysicalDevice physical_device, VkFormat* depth_format);
		static VkSamplerCreateInfo SamplerCreateInfo();

		static VkPipelineShaderStageCreateInfo PipelineShaderStageCreateInfo(VkShaderStageFlagBits stage, VkShaderModule shader_module);
		static VkPipelineVertexInputStateCreateInfo PipelineVertexInputStageCreateInfo();
		static VkPipelineInputAssemblyStateCreateInfo PipelineInputAssemblyStateCreateInfo(VkPrimitiveTopology topology);
		static VkPipelineRasterizationStateCreateInfo  PipelineRasterizationStateCreateInfo(VkPolygonMode polygon_mode, VkCullModeFlags cull_mode);
		static VkPipelineMultisampleStateCreateInfo PipelineMultisampleStateCreateInfo();
		static VkPipelineColorBlendAttachmentState  PipelineColorBlendAttachmentState(VkColorComponentFlags color_write_mask,VkBool32 blend_enable);
		static VkPipelineLayoutCreateInfo PipelineLayoutCreateInfo(VkDescriptorSetLayout* p_set_layouts, uint32_t layout_count, VkPushConstantRange* p_constant_ranges, uint32_t constant_count);
		static VkPipelineDepthStencilStateCreateInfo DepthStencilCreateInfo(bool b_depth_test, bool b_depth_write, VkCompareOp compare_op);
	    static  VkPipelineVertexInputStateCreateInfo PipelineVertexInputStateCreateInfo();

		static VkSemaphoreCreateInfo SemaphoreCreateInfo();
		static VkRenderPassBeginInfo RenderPassBeginInfo();
		static VkCommandBufferBeginInfo CommandBufferBeginInfo();
	};
}






#endif //!VULKAN_INITIALIZER_H