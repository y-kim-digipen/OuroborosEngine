#include "vulkan_material.h"

#include "vulkan_buffer.h"

namespace Renderer {

	//TODO: Material(*material) ???
	VulkanMaterial::VulkanMaterial(Vulkan_type* vulkan_type, Material* material) : Material(*material), vulkan_type(vulkan_type), ubo(std::make_unique<VulkanUniformBuffer>(vulkan_type, sizeof(Material)))
	{
		//TODO: might not need to create descriptor set layout everytime
		VkDescriptorSetLayoutBinding binding;
		binding.binding = 2;
		binding.descriptorCount = 1;
		binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		
		VkDescriptorSetLayoutCreateInfo set_layout_create_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO };
		set_layout_create_info.bindingCount = 1;
		set_layout_create_info.pBindings = &binding;

		VkDescriptorSetLayout set_layout{};

		VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type->device.handle, &set_layout_create_info, 0, &set_layout));

		//TODO: dont use magic number
		ubo->SetupDescriptorSet(2, 1, set_layout);
	}

	VulkanMaterial::~VulkanMaterial()
	{
	}

	void VulkanMaterial::Bind()
	{
		ubo->Bind();
	}
}