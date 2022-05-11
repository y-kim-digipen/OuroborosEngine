#include "Graphics/vulkan/vulkan_context.h"
#include "vulkan_type.inl"

#include <iostream>

#include <optional>


namespace Renderer 
{
    static Vulkan_type vulkan_type;

    struct QueueFamilyIndices
	{
       std::optional<uint32_t> graphicsFamily;
        bool isComplete()
    	{
            return graphicsFamily.has_value();
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

    int CreateSurface()
    {



        return 0;
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

        int index = 0;
        for (const auto& queueFamily : queueFamilies) 
        {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            {
                indices.graphicsFamily = index;
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

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;

        float queuePriority = 1.0f;
        queueCreateInfo.pQueuePriorities = &queuePriority;

        VkPhysicalDeviceFeatures deviceFeatures{};

        VkDeviceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.queueCreateInfoCount = 1;

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

        vkGetDeviceQueue(vulkan_type.device.handle, indices.graphicsFamily.value(), 0, &vulkan_type.device.graphicsQueue);

        return 0;

    }
}

