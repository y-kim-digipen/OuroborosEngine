#include "VulkanContext.h"
#include "vulkan_type.inl"
#include <vulkan_core.h>

#include <iostream>
#include <optional>
#include <set>
#include <limits> 

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
//#define GLFW_EXPOSE_NATIVE_WIN32s
//#include <GLFW/glfw3native.h>


namespace Renderer 
{
    static Vulkan_type vulkan_type;


    const std::vector<const char*> device_extensions = 
    {
	  VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };


    const std::vector<const char*> validation_layers =
    {
        "VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

	int CreateInstance(int major, int minor);
    void CreateDebugUtilMessage();
    int CreateSurface();
    int PickPhysicalDevice();
    int CreateLogicalDevice();
    int CreateImageView();


    bool IsDevicesSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);




    static VKAPI_ATTR VkBool32 debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        switch (messageSeverity) {
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
            std::cout << ("error: %s", pCallbackData->pMessage) << std::endl;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
            std::cout << ("warning: %s", pCallbackData->pMessage) << std::endl;
            break;
        case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
            std::cout << ("info: %s", pCallbackData->pMessage) << std::endl;
            break;
        default:
            std::cout << ("info: %s", pCallbackData->pMessage) << std::endl;
        }

        return VK_FALSE;
    }

	void VulkanContext::Init(int major, int minor) 
	{
        CreateInstance(major, minor);
        CreateDebugUtilMessage();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSurface();
        CreateSwapChain();

	}

    void VulkanContext::Shutdown()
    {

        for (auto image_view : vulkan_type.swap_chain.swap_chain_image_views) 
        {
            vkDestroyImageView(vulkan_type.device.handle, image_view, nullptr);
        }
        vkDestroySwapchainKHR(vulkan_type.device.handle, vulkan_type.swap_chain.handle, nullptr);
        vkDestroyDevice(vulkan_type.device.handle, nullptr);
        vkDestroySurfaceKHR(vulkan_type.instance, vulkan_type.surface, 0);


    }

    void VulkanContext::CreateSurface()
    {
        if (glfwCreateWindowSurface(vulkan_type.instance, window, nullptr, &vulkan_type.surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanContext::CreateSwapChain()
    {
        SwapChainSupportDetails swap_chain_support = querySwapChainSupport(vulkan_type.device.physical_device);

        VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
        VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities,window);
        uint32_t image_count = swap_chain_support.capabilities.minImageCount + 1;
        if (swap_chain_support.capabilities.maxImageCount > 0 && image_count > swap_chain_support.capabilities.maxImageCount) {
            image_count = swap_chain_support.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        create_info.surface = vulkan_type.surface;

        create_info.minImageCount = image_count;
        create_info.imageFormat = surface_format.format;
        create_info.imageColorSpace = surface_format.colorSpace;
        create_info.imageExtent = extent;
        create_info.imageArrayLayers = 1;
        create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = FindQueueFamilies(vulkan_type.device.physical_device);
        uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

        if (indices.graphics_family != indices.present_family) {
            create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            create_info.queueFamilyIndexCount = 2;
            create_info.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        create_info.preTransform = swap_chain_support.capabilities.currentTransform;
        create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        create_info.presentMode = present_mode;
        create_info.clipped = VK_TRUE;

        create_info.oldSwapchain = VK_NULL_HANDLE;


        if (vkCreateSwapchainKHR(vulkan_type.device.handle, &create_info, nullptr, &vulkan_type.swap_chain.handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(vulkan_type.device.handle, vulkan_type.swap_chain.handle, &image_count, nullptr);

    	vulkan_type.swap_chain.swap_chain_images.resize(image_count);
        vkGetSwapchainImagesKHR(vulkan_type.device.handle, vulkan_type.swap_chain.handle, &image_count, vulkan_type.swap_chain.swap_chain_images.data());

        vulkan_type.swap_chain.swap_chain_image_format = surface_format.format;
        vulkan_type.swap_chain.swap_chain_extent = extent;
    }

    int CreateInstance(int major, int minor)
    {
        VkApplicationInfo app_info{ VK_STRUCTURE_TYPE_APPLICATION_INFO };
        app_info.pApplicationName = "Ouroboros-engine";
        app_info.applicationVersion = VK_MAKE_VERSION(major, minor, 0);
        app_info.pEngineName = "Ouroboros_ENGINE";
        app_info.engineVersion = VK_MAKE_VERSION(major, minor, 0);
        app_info.apiVersion = VK_MAKE_VERSION(major, minor, 0);

        VkInstanceCreateInfo instance_create_info{ VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO };
        instance_create_info.pApplicationInfo = &app_info;

#if defined(_DEBUG)
        const char* required_layer_names[] = 
        {
           "VK_LAYER_KHRONOS_validation"
        };

        unsigned required_layer_count = sizeof(required_layer_names) / sizeof(const char*);

        unsigned available_layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
        std::vector<VkLayerProperties> available_properties(available_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_properties.data()));

        for (unsigned i = 0; i < required_layer_count; ++i) 
        {
            std::cout << "Searching validation layer: " << required_layer_names[i] << "..." << std::endl;
            bool is_found = false;;
            for (unsigned l = 0; l < available_layer_count; ++l) 
            {
                if (strcmp(required_layer_names[i], available_properties.at(l).layerName) == 0) 
                {
                    std::cout << "found" << std::endl;
                    is_found = true;
                    break;
                }
            }

            if (!is_found) 
            {
                std::cout << required_layer_names[i] << "is missing!\n";
                return false;
            }
        }

        instance_create_info.enabledLayerCount = required_layer_count;
        instance_create_info.ppEnabledLayerNames = required_layer_names;
#endif //_DEBUG

        //TODO: this should be formatted as a function of inside the platform
        unsigned extension_count = 0;
        const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&extension_count);

        std::vector<const char*> required_extension_names(glfw_extensions, glfw_extensions + extension_count);
#if defined(_DEBUG)
        required_extension_names.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        ++extension_count;
#endif //_DEBUG

        instance_create_info.enabledExtensionCount = extension_count;
        instance_create_info.ppEnabledExtensionNames = required_extension_names.data();

        VK_CHECK(vkCreateInstance(&instance_create_info, 0, &vulkan_type.instance));
        return 0;
    }

    void CreateDebugUtilMessage()
    {
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT;
        debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
        debugCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;
        debugCreateInfo.pUserData = 0;
        ;
        //vkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger);


        //because of the static library
        PFN_vkCreateDebugUtilsMessengerEXT myvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vulkan_type.instance, "vkCreateDebugUtilsMessengerEXT"));


        myvkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger);
    	//VK_CHECK(vkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger));
    }

    int PickPhysicalDevice()
    {
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;
        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vulkan_type.instance, &device_count, nullptr);

        if (device_count == 0) 
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(vulkan_type.instance, &device_count, devices.data()));

    	for (const auto& device : devices)
        {
            if (IsDevicesSuitable(device)) 
            {
                physical_device = device;
                break;
            }
        }

        if (physical_device == VK_NULL_HANDLE) 
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

    	VkPhysicalDeviceProperties physical_properties = {};
    	vkGetPhysicalDeviceProperties(physical_device, &physical_properties);

    	fprintf(stdout, "Device Name:    %s\n", physical_properties.deviceName);
    	fprintf(stdout, "Device Type:    %d\n", physical_properties.deviceType);
    	fprintf(stdout, "Driver Version: %d\n", physical_properties.driverVersion);

    	fprintf(stdout, "API Version:    %d.%d.%d\n",
                VK_VERSION_MAJOR(physical_properties.apiVersion),
                VK_VERSION_MINOR(physical_properties.apiVersion),
                VK_VERSION_PATCH(physical_properties.apiVersion));
        

        return 0;
    }


    bool IsDevicesSuitable(VkPhysicalDevice device)
	{
        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensions_supported = CheckDeviceExtensionSupport(device);
        bool swap_chain_adequate = false;

        if (extensions_supported) 
        {
            SwapChainSupportDetails swap_chain_support = querySwapChainSupport(device);
            swap_chain_adequate = !swap_chain_support.formats.empty() && !swap_chain_support.present_modes.empty();
        }


        return indices.isComplete() && extensions_supported && swap_chain_adequate;

    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);

        std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, queue_families.data());
        

        int index = 0;
        for (const auto& queue_family : queue_families) 
        {

            if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphics_family = index;
            }

            VkBool32 present_support = false;

            auto temp_pointer = vkGetInstanceProcAddr(vulkan_type.instance, "vkGetPhysicalDeviceSurfaceSupportKHR");

            PFN_vkGetPhysicalDeviceSurfaceSupportKHR myvkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)temp_pointer;
            myvkGetPhysicalDeviceSurfaceSupportKHR(device, index, vulkan_type.surface, &present_support);

            //vkGetPhysicalDeviceSurfaceSupportKHR(device, index, vulkan_type.surface, &present_support);

            if (present_support) 
            {
                indices.present_family = index;
            }

            if (indices.isComplete()) 
            {
                break;
            }
            index++;
        }

        return indices;
    }

    int CreateLogicalDevice()
    {
        QueueFamilyIndices indices = FindQueueFamilies(vulkan_type.device.physical_device);
        
        std::set<uint32_t> unique_queue_indices = {
            indices.graphics_family.value(), indices.present_family.value()
        };

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        for (uint32_t queue_family : unique_queue_indices) {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;

            float queuePriority = 1.0f;
            queue_create_info.pQueuePriorities = &queuePriority;
            queue_create_infos.push_back(queue_create_info);
        }
        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        
        createInfo.pQueueCreateInfos = queue_create_infos.data();
        createInfo.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        createInfo.ppEnabledExtensionNames = device_extensions.data();
        
        if (enable_validation_layers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            createInfo.ppEnabledLayerNames = validation_layers.data();
        }
        else 
        {
            createInfo.enabledLayerCount = 0;
        }

        if (vkCreateDevice(vulkan_type.device.physical_device, &createInfo, nullptr, &vulkan_type.device.handle) != VK_SUCCESS) 
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(vulkan_type.device.handle, indices.graphics_family.value(), 0, &vulkan_type.device.graphics_queue);
        vkGetDeviceQueue(vulkan_type.device.handle, indices.present_family.value(), 0, &vulkan_type.device.present_queue);

        return 0;
    }

    int CreateImageView()
    {
    	vulkan_type.swap_chain.swap_chain_image_views.resize(vulkan_type.swap_chain.swap_chain_images.size());

        for (size_t i = 0; i < vulkan_type.swap_chain.swap_chain_images.size(); i++) {
            VkImageViewCreateInfo createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image = vulkan_type.swap_chain.swap_chain_images[i];
            createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format = vulkan_type.swap_chain.swap_chain_image_format;
            createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vulkan_type.device.handle, &createInfo, nullptr, &vulkan_type.swap_chain.swap_chain_image_views[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }

        return 0;
    }


    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
	{
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, vulkan_type.surface, &details.capabilities);

        uint32_t format_count;
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan_type.surface, &format_count, nullptr);

        if (format_count != 0) 
        {
            details.formats.resize(format_count);
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, vulkan_type.surface, &format_count, details.formats.data());
        }

        uint32_t present_mode_count;
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan_type.surface, &present_mode_count, nullptr);

        if (present_mode_count != 0) 
        {
            details.present_modes.resize(present_mode_count);
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, vulkan_type.surface, &present_mode_count, details.present_modes.data());
        }

        return details;
    }


    bool CheckDeviceExtensionSupport(VkPhysicalDevice device)
    {
        uint32_t extension_count;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);

        std::vector<VkExtensionProperties> available_extensions(extension_count);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, available_extensions.data());

        std::set<std::string> requiredExtensions(device_extensions.begin(), device_extensions.end());

        for (const auto& extension : available_extensions) 
        {
            requiredExtensions.erase(extension.extensionName);
        }

        return requiredExtensions.empty();
    }

    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats)
	{
        for (const auto& available_format : available_formats) 
        {
            if (available_format.format == VK_FORMAT_B8G8R8A8_SRGB && available_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) 
            {
                return available_format;
            }
        }

        return available_formats[0];
    }

    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes)
	{
        for (const auto& available_present_mode : available_present_modes) 
        {
            if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR) 
            {
                return available_present_mode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }


    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
	{
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return capabilities.currentExtent;
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actual_extent =
            {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
            actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

            return actual_extent;
        }
    }

}

