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
	};
}






#endif //!VULKAN_INITIALIZER_H