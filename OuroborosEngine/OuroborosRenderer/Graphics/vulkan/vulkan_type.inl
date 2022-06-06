#pragma once

#include <vulkan.h>
#include <vulkan_core.h>
//#include <vulkan_win32.h>

#include <vector>
#include <cassert>
#include <optional>
#include <vk_mem_alloc.h>

#define VK_CHECK(call) \
	do {	\
		VkResult result_ = call; \
		assert(result_ == VK_SUCCESS); \
	} while(0)

constexpr int MAX_FRAMES_IN_FLIGHT = 3;

struct VulkanDevice
{
	VkPhysicalDevice physical_device;
	VkDevice handle;
	VkQueue graphics_queue;
	VkQueue present_queue;
};

struct SwapchainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> present_modes;
};

struct VulkanImage {
	VmaAllocation allocation;
	VkImage image;
	VkImageView image_view;
	VkFormat format;
};

struct VulkanSwapchain
{
	SwapchainSupportDetails detail;
	VkFormat image_format;
	VkExtent2D extent;
	VkSwapchainKHR handle;
	std::vector<VkImage> images;
	std::vector<VkImageView> image_views;
	VulkanImage depth_image;
	std::vector<VkFramebuffer> framebuffers;
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

//Maybe use
struct VulkanCommand
{
	VkCommandPool command_pool;
	VkCommandBuffer command_buffer;
};

struct VulkanSemaphore
{
	VkSemaphore image_available_semaphore;
	VkSemaphore render_finished_semaphore;
	VkFence in_flight_fence;
};

struct VulkanFrameData
{
	VulkanSemaphore semaphore;
	VkCommandBuffer command_buffer;

	uint32_t swap_chain_image_index{ 0 };
};

struct Vulkan_type
{
	
#if defined(_DEBUG)
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
	
	VkInstance instance;
	VulkanDevice device;
	VkSurfaceKHR surface;
	VulkanSwapchain swapchain;
	VkRenderPass render_pass;

	VkDescriptorPool descriptor_pool;
	VmaAllocator allocator;
	VulkanFrameData frame_data[MAX_FRAMES_IN_FLIGHT];

	uint32_t current_frame = 0;

	VkCommandPool command_pool;


	//TODO: temp global pipeline layout
	VkPipelineLayout global_pipeline_layout;
	VkPipelineLayout current_pipeline_layout = VK_NULL_HANDLE;
};