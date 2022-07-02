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
}
