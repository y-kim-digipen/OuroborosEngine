#pragma once

#include <vulkan.h>

#include <vector>
#include <cassert>

#define VK_CHECK(call) \
	do {	\
		VkResult result_ = call; \
		assert(result_ == VK_SUCCESS); \
	} while(0)

struct Vulkan_Device
{
	VkPhysicalDevice physical_device;
	VkDevice handle;
	VkQueue graphicsQueue;
};

struct Vulkan_type
{

#if defined(_DEBUG)
	VkDebugUtilsMessengerEXT debug_messenger;
#endif
	
	VkInstance instance;
	Vulkan_Device device;
};