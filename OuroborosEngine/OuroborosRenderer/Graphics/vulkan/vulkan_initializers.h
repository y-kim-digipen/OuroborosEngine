#ifndef VULKAN_INITIALIZER_H
#define VULKAN_INITIALIZER_H
#include "vulkan_type.inl"

namespace Renderer
{
	class VulkanInitializer
	{
	public:
		static VkDescriptorSetLayoutBinding DescripterSetLayoutBinding(VkDescriptorType type, VkShaderStageFlags stage_flags, uint32_t binding, uint32_t descriptor_count);
		static VkDescriptorSetLayoutCreateInfo DescriptorSetLayoutCreateInfo(const VkDescriptorSetLayoutBinding* p_bindings, uint32_t binding_count);
		static VkSemaphoreCreateInfo SemaphoreCreateInfo();
		static VkCommandBufferBeginInfo CommandBufferBeginInfo();
		static VkRenderPassBeginInfo RenderPassBeginInfo();
		static VkBool32 GetSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);
		static VkSamplerCreateInfo SamplerCreateInfo();
		static VkDescriptorSetAllocateInfo DescriptorSetAllocateInfo(VkDescriptorPool descriptor_pool,const VkDescriptorSetLayout* p_set_layouts,uint32_t descriptor_set_count);
		static VkDescriptorImageInfo DescriptorImageInfo(VkSampler sampler, VkImageView image_view, VkImageLayout image_layout);
		static VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dst_set,VkDescriptorType type,uint32_t binding,VkDescriptorBufferInfo* buffer_info,uint32_t descriptor_count = 1);
		static VkWriteDescriptorSet WriteDescriptorSet(VkDescriptorSet dst_set, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* image_info, uint32_t descriptor_count = 1);
		
	};

}



#endif //!VULKAN_INITIALIZER_H