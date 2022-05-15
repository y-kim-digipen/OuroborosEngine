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
	VkPipelineLayout pipeline_layout;
};


namespace Renderer
{
	static Vulkan_type vulkan_type;
}