#include "vulkan_context.h"
#include "vulkan_type.inl"

#include <iostream>
#include <optional>
#include <set>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>

namespace Renderer 
{
    static Vulkan_type vulkan_type;

    struct QueueFamilyIndices
	{
       std::optional<uint32_t> graphics_family;
       std::optional<uint32_t> present_family;
        bool isComplete()
    	{
            return graphics_family.has_value() && present_family.has_value();
        }
    };

    const std::vector<const char*> validationLayers =
    {
        "VK_LAYER_KHRONOS_validation"
	};

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif

	int CreateInstance(int major, int minor);
    void CreateDebugUtilMessage();
    int CreateSurface();
    int PickPhysicalDevice();
    bool IsDevicesSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    int CreateLogicalDevice();


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
	void Vulkan_Context::Init(int major, int minor) 
	{
        CreateInstance(major, minor);
        CreateDebugUtilMessage();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateSurface();
	}

    void Vulkan_Context::Shutdown()
    {

        vkDestroySurfaceKHR(vulkan_type.instance, vulkan_type.surface, 0);

    }

    void Vulkan_Context::CreateSurface()
    {
        if (glfwCreateWindowSurface(vulkan_type.instance, window, nullptr, &vulkan_type.surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
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
        const char* required_layer_names[] = {
           "VK_LAYER_KHRONOS_validation"
        };

        unsigned required_layer_count = sizeof(required_layer_names) / sizeof(const char*);

        unsigned available_layer_count = 0;
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, nullptr));
        std::vector<VkLayerProperties> available_properties(available_layer_count);
        VK_CHECK(vkEnumerateInstanceLayerProperties(&available_layer_count, available_properties.data()));

        for (unsigned i = 0; i < required_layer_count; ++i) {
            std::cout << "Searching validation layer: " << required_layer_names[i] << "..." << std::endl;
            bool is_found = false;;
            for (unsigned l = 0; l < available_layer_count; ++l) {
                if (strcmp(required_layer_names[i], available_properties.at(l).layerName) == 0) {
                    std::cout << "found" << std::endl;
                    is_found = true;
                    break;
                }
            }

            if (!is_found) {
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

        VK_CHECK(vkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger));
    }

    int PickPhysicalDevice()
    {
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(vulkan_type.instance, &deviceCount, nullptr);

        if (deviceCount == 0) 
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(vulkan_type.instance, &deviceCount, devices.data());

    	for (const auto& device : devices)
        {
            if (IsDevicesSuitable(device)) 
            {
                physicalDevice = device;
                break;
            }
        }

        if (physicalDevice == VK_NULL_HANDLE) 
        {
            throw std::runtime_error("failed to find a suitable GPU!");
        }

    	VkPhysicalDeviceProperties physicalProperties = {};
    	vkGetPhysicalDeviceProperties(physicalDevice, &physicalProperties);

    	fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
    	fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
    	fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);

    	fprintf(stdout, "API Version:    %d.%d.%d\n",
                VK_VERSION_MAJOR(physicalProperties.apiVersion),
                VK_VERSION_MINOR(physicalProperties.apiVersion),
                VK_VERSION_PATCH(physicalProperties.apiVersion));
        

        return 0;
    }


    bool IsDevicesSuitable(VkPhysicalDevice device)
	{
        QueueFamilyIndices indices = FindQueueFamilies(device);

        return indices.isComplete();

    }

    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device)
    {
        QueueFamilyIndices indices;

        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        //vkGetPhysicalDeviceSurfaceSupportKHR()

        int index = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, index, vulkan_type.surface, &presentSupport);

            if (presentSupport) {
                indices.present_family = index;
            }

            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphics_family = index;
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
        createInfo.queueCreateInfoCount = queue_create_infos.size();

        createInfo.pEnabledFeatures = &deviceFeatures;

        createInfo.enabledExtensionCount = 0;

        if (enableValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();
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
}

