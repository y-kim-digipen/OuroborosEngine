#include "vulkan_texture.h"

namespace Renderer
{
	VulkanTexture::VulkanTexture(Vulkan_type* vulkan_type) : vulkan_type(vulkan_type)
	{

	}

	void VulkanTexture::Bind()
	{


	}

	void VulkanTexture::UploadData(const Asset::Image& data)
	{
		Texture::UploadData(data);

		uint32_t width = data.width;
		uint32_t height = data.height;

		
		if(width != width_ || height != height_)
		{
			VkFilter filter = VK_FILTER_NEAREST;
			VkSamplerAddressMode sampler_address_mode = VK_SAMPLER_ADDRESS_MODE_REPEAT;

			VkSamplerCreateInfo sampler_create_info
			{
				.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
				.magFilter = filter,
				.minFilter = filter,
				.addressModeU = sampler_address_mode,
				.addressModeV = sampler_address_mode,
				.addressModeW = sampler_address_mode,
			};

			vkCreateSampler(vulkan_type->device.handle, &sampler_create_info, nullptr, &sampler_);







		}



	}
}
