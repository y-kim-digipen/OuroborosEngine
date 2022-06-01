#include "vulkan_context.h"

#define VMA_IMPLEMENTATION
#include "vulkan_type.inl"
#include "vulkan_image.h"
#include "vulkan_shader.h"
#include "vulkan_mesh.h"

#include <iostream>
#include <optional>
#include <set>
#include <limits> 

#include "spirv_helper.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>

#include "backends/imgui_impl_vulkan.h"
//#define GLFW_EXPOSE_NATIVE_WIN32s
//#include <GLFW/glfw3native.h>

static Vulkan_type vulkan_type;

namespace Renderer
{

#ifdef NDEBUG
    const bool enable_validation_layers = false;
#else
    const bool enable_validation_layers = true;
#endif

    int CreateInstance(int major, int minor);
    void CreateDebugUtilMessage();
    int PickPhysicalDevice();
    int CreateLogicalDevice();
    int CreateVMAallocator();
    int CreateSwapchainImageView();
    int CreateRenderPass();
    int CreateFrameBuffers();
    int CreateCommandPool();
    int CreateCommandBuffer();
    void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index);
    int CreateSyncObjects();
    int CreateDescriptorPool();
    void CleanupSwapChain();
    int RateDeviceSuitability(VkPhysicalDevice device);

    bool IsDevicesSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    SwapchainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& available_formats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& available_present_modes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindDepthFormat();



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
        SpirvHelper::Init();
        CreateInstance(major, minor);
#if defined(_DEBUG)
        CreateDebugUtilMessage();
#endif //_DEBUG
        CreateSurface();
        PickPhysicalDevice();
        CreateLogicalDevice();
        CreateVMAallocator();
        CreateSwapChain();
        CreateSwapchainImageView();
        CreateRenderPass();
        CreateFrameBuffers();
        CreateCommandPool();
        CreateCommandBuffer();
        CreateSyncObjects();
        CreateDescriptorPool();

    }

    void VulkanContext::Shutdown()
    {
        vkDeviceWaitIdle(vulkan_type.device.handle);

        for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
        {
            auto& frame_data = vulkan_type.frame_data[i];
            vkDestroySemaphore(vulkan_type.device.handle, frame_data.semaphore.render_finished_semaphore, nullptr);
            vkDestroySemaphore(vulkan_type.device.handle, frame_data.semaphore.image_available_semaphore, nullptr);
            vkDestroyFence(vulkan_type.device.handle, frame_data.semaphore.in_flight_fence, nullptr);
        }


        for (auto framebuffer : vulkan_type.swapchain.framebuffers)
        {
            vkDestroyFramebuffer(vulkan_type.device.handle, framebuffer, nullptr);
        }
        //TODO : need to implement in shader for destorying the pipeline layout
        //vkDestroyPipelineLayout(vulkan_type.device.handle, vulkan_type.pipeline_layout, nullptr);
        vkDestroyRenderPass(vulkan_type.device.handle, vulkan_type.render_pass, nullptr);

        for (auto image_view : vulkan_type.swapchain.image_views)
        {
            vkDestroyImageView(vulkan_type.device.handle, image_view, nullptr);
        }
        vkDestroySwapchainKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, nullptr);
        vkDestroyDevice(vulkan_type.device.handle, nullptr);


        vkDestroySurfaceKHR(vulkan_type.instance, vulkan_type.surface, 0);


        SpirvHelper::Finalize();

    }

    int VulkanContext::AddShader(ShaderConfig* config)
    {
        if (shader_map.find(config->name) != shader_map.end())
            return -1;
        else {
            shader_map[config->name] = std::make_unique<VulkanShader>(&vulkan_type);
            shader_map[config->name]->Init(config);
        }

        return 0;
    }

    int VulkanContext::AddMesh(const char* mesh_name)
    {
        if (mesh_map.find(mesh_name) != mesh_map.end()) {
            std::cout << mesh_name << " already exists\n";
            return -1;
        }

        mesh_map[mesh_name] = std::make_unique<VulkanMesh>(&vulkan_type);
        if (!mesh_map[mesh_name]->LoadAsset(mesh_name)) {
            std::cout << mesh_name << " loading failed\n";
            return -1;
        }

        return 0;
    }

    void VulkanContext::DrawMeshes(const std::vector<const char*>& shaders_name, const std::vector<const char*>& meshes_name)
    {

        uint32_t mesh_name_count = meshes_name.size();

        for (uint32_t i = 0; i < mesh_name_count; ++i) {

            shader_map[shaders_name[i]]->Bind();

        }
    }

    void VulkanContext::DrawMesh(const char* shader_name, const char* mesh_name)
    {
        shader_map[shader_name]->Bind();

        mesh_map[mesh_name]->Draw();
    }

    void VulkanContext::CreateSurface()
    {
        if (glfwCreateWindowSurface(vulkan_type.instance, window, nullptr, &vulkan_type.surface) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanContext::CreateSwapChain()
    {
        SwapchainSupportDetails swap_chain_support = querySwapChainSupport(vulkan_type.device.physical_device);

        VkSurfaceFormatKHR surface_format = ChooseSwapSurfaceFormat(swap_chain_support.formats);
        VkPresentModeKHR present_mode = ChooseSwapPresentMode(swap_chain_support.present_modes);
        VkExtent2D extent = ChooseSwapExtent(swap_chain_support.capabilities, window);
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


        if (vkCreateSwapchainKHR(vulkan_type.device.handle, &create_info, nullptr, &vulkan_type.swapchain.handle) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, &image_count, nullptr);

        vulkan_type.swapchain.images.resize(image_count);
        vkGetSwapchainImagesKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, &image_count, vulkan_type.swapchain.images.data());

        vulkan_type.swapchain.image_format = surface_format.format;
        vulkan_type.swapchain.extent = extent;


    }

    void VulkanContext::RecreateSwapChain()
    {
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);

        while (width == 0|| height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }
        
        vkDeviceWaitIdle(vulkan_type.device.handle);
        CleanupSwapChain();
        CreateSwapChain();
        CreateSwapchainImageView();
        CreateRenderPass();
        CreateFrameBuffers();
    }

    int VulkanContext::BeginFrame()
    {
        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
        VK_CHECK(vkWaitForFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence, VK_TRUE, UINT64_MAX));

        VkResult result = vkAcquireNextImageKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, UINT64_MAX, frame_data.semaphore.image_available_semaphore, VK_NULL_HANDLE, &frame_data.swap_chain_image_index);

            
        VK_CHECK(vkResetFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence));



        VK_CHECK(vkResetCommandBuffer(frame_data.command_buffer, 0));
        RecordCommandBuffer(frame_data.command_buffer, frame_data.swap_chain_image_index);

        return 0;
    }

    int VulkanContext::EndFrame()
    {

        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
        vkCmdEndRenderPass(frame_data.command_buffer);
        vkEndCommandBuffer(frame_data.command_buffer);

        auto& semaphore = frame_data.semaphore;
        VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        submit.pWaitDstStageMask = &wait_stage;

        //TODO :CHECK IS IT right pWaitsemaphore = image_available_semaphore
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &semaphore.image_available_semaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &semaphore.render_finished_semaphore;

        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &frame_data.command_buffer;

        vkQueueSubmit(vulkan_type.device.graphics_queue, 1, &submit, frame_data.semaphore.in_flight_fence);

        VkPresentInfoKHR present_info_khr{ VK_STRUCTURE_TYPE_PRESENT_INFO_KHR };
        present_info_khr.swapchainCount = 1;
        present_info_khr.pSwapchains = &vulkan_type.swapchain.handle;
        present_info_khr.waitSemaphoreCount = 1;
        present_info_khr.pWaitSemaphores = &semaphore.render_finished_semaphore;
        present_info_khr.pImageIndices = &frame_data.swap_chain_image_index;

        auto result = vkQueuePresentKHR(vulkan_type.device.graphics_queue, &present_info_khr);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
        {
            RecreateSwapChain();
        }

        vulkan_type.current_frame = (vulkan_type.current_frame + 1) % MAX_FRAMES_IN_FLIGHT;

        return  0;
    }

    Vulkan_type* VulkanContext::GetVulkanType()
    {
        return &vulkan_type;
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
#if defined(_DEBUG)
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


        VK_CHECK(myvkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger));
        //VK_CHECK(vkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger));
    }
#endif

    int PickPhysicalDevice()
    {
        VkPhysicalDevice physical_device = VK_NULL_HANDLE;

        //auto physical_device = vulkan_type.device.physical_device;

        physical_device = VK_NULL_HANDLE;

        uint32_t device_count = 0;
        vkEnumeratePhysicalDevices(vulkan_type.instance, &device_count, nullptr);

        if (device_count == 0)
        {
            throw std::runtime_error("failed to find GPUs with Vulkan support!");
        }

        std::vector<VkPhysicalDevice> devices(device_count);
        VK_CHECK(vkEnumeratePhysicalDevices(vulkan_type.instance, &device_count, devices.data()));

        //vulkan_type.device.physical_device = devices[0];

        std::multimap<int, VkPhysicalDevice> candidates;

        for (const auto& device : devices)
        {
            if (IsDevicesSuitable(device))
            {
                int score = RateDeviceSuitability(device);
                candidates.insert(std::make_pair(score, device));
            }
        }

        if (candidates.rbegin()->first > 0)
        {
            physical_device = candidates.rbegin()->second;

        }
        else
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

        vulkan_type.device.physical_device = physical_device;

        return 0;
    }


    bool IsDevicesSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties device_property;
        vkGetPhysicalDeviceProperties(device, &device_property);

        if (device_property.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            return false;
        }

        QueueFamilyIndices indices = FindQueueFamilies(device);

        bool extensions_supported = CheckDeviceExtensionSupport(device);
        bool swap_chain_adequate = false;

        if (extensions_supported)
        {
            SwapchainSupportDetails swap_chain_support = querySwapChainSupport(device);
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

            auto function = vkGetInstanceProcAddr(vulkan_type.instance, "vkGetPhysicalDeviceSurfaceSupportKHR");

            PFN_vkGetPhysicalDeviceSurfaceSupportKHR myvkGetPhysicalDeviceSurfaceSupportKHR = (PFN_vkGetPhysicalDeviceSurfaceSupportKHR)function;

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

        std::set<uint32_t> unique_queue_indices =
        {
            indices.graphics_family.value(), indices.present_family.value()
        };

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos;

        for (uint32_t queue_family : unique_queue_indices)
        {
            VkDeviceQueueCreateInfo queue_create_info{};
            queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount = 1;

            float queue_priority = 1.0f;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }
        VkPhysicalDeviceFeatures device_features{};

        VkDeviceCreateInfo create_info{};
        create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

        create_info.pQueueCreateInfos = queue_create_infos.data();
        create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());

        create_info.pEnabledFeatures = &device_features;


        const std::vector<const char*> device_extensions =
        {
          VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        create_info.enabledExtensionCount = static_cast<uint32_t>(device_extensions.size());
        create_info.ppEnabledExtensionNames = device_extensions.data();

        const std::vector<const char*> validation_layers =
        {
            "VK_LAYER_KHRONOS_validation"
        };

        if (enable_validation_layers)
        {
            create_info.enabledLayerCount = static_cast<uint32_t>(validation_layers.size());
            create_info.ppEnabledLayerNames = validation_layers.data();
        }
        else
        {
            create_info.enabledLayerCount = 0;
        }

        if (vkCreateDevice(vulkan_type.device.physical_device, &create_info, nullptr, &vulkan_type.device.handle) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create logical device!");
        }

        vkGetDeviceQueue(vulkan_type.device.handle, indices.graphics_family.value(), 0, &vulkan_type.device.graphics_queue);
        vkGetDeviceQueue(vulkan_type.device.handle, indices.present_family.value(), 0, &vulkan_type.device.present_queue);

        return 0;
    }

    int CreateSwapchainImageView()
    {
        vulkan_type.swapchain.image_views.resize(vulkan_type.swapchain.images.size());

        for (size_t i = 0; i < vulkan_type.swapchain.images.size(); i++)
        {
            VkImageViewCreateInfo create_info{};
            create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            create_info.image = vulkan_type.swapchain.images[i];
            create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
            create_info.format = vulkan_type.swapchain.image_format;
            create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
            create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            create_info.subresourceRange.baseMipLevel = 0;
            create_info.subresourceRange.levelCount = 1;
            create_info.subresourceRange.baseArrayLayer = 0;
            create_info.subresourceRange.layerCount = 1;

            if (vkCreateImageView(vulkan_type.device.handle, &create_info, nullptr, &vulkan_type.swapchain.image_views[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }

        vulkan_type.swapchain.depth_image.format = FindDepthFormat();

        CreateImage(&vulkan_type, &vulkan_type.swapchain.depth_image,
            VK_IMAGE_TYPE_2D,
            vulkan_type.swapchain.extent.width,
            vulkan_type.swapchain.extent.height,
            vulkan_type.swapchain.depth_image.format,
            VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
            VK_IMAGE_TILING_OPTIMAL,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            true,
            VK_IMAGE_ASPECT_DEPTH_BIT);

        return 0;
    }

    int CreateRenderPass()
    {
        VkAttachmentDescription color_attachment{};
        color_attachment.format = vulkan_type.swapchain.image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentDescription depth_attachment{};
        depth_attachment.format = vulkan_type.swapchain.depth_image.format;
        depth_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depth_attachment_ref{};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorAttachmentRef;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        const uint32_t attachment_count = 2;
        VkAttachmentDescription attachment[attachment_count] = {
            color_attachment,
            depth_attachment
        };

        VkSubpassDependency dependency{};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;

        VkSubpassDependency depth_dependency{};
        depth_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        depth_dependency.dstSubpass = 0;
        depth_dependency.srcStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        depth_dependency.srcAccessMask = 0;
        depth_dependency.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;

        const uint32_t dependency_count = 2;
        VkSubpassDependency dependencies[dependency_count] = {
            dependency,
            depth_dependency
        };

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount = attachment_count;
        render_pass_info.pAttachments = attachment;
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;
        render_pass_info.dependencyCount = dependency_count;
        render_pass_info.pDependencies = dependencies;

        if (vkCreateRenderPass(vulkan_type.device.handle, &render_pass_info, nullptr, &vulkan_type.render_pass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }

        return 0;
    }

    int CreateFrameBuffers()
    {
        auto& swapchain = vulkan_type.swapchain;
        swapchain.framebuffers.resize(swapchain.image_views.size());

        for (size_t idx = 0; idx < swapchain.image_views.size(); idx++)
        {
            VkImageView attachments[] = { swapchain.image_views.at(idx), swapchain.depth_image.image_view };

            VkFramebufferCreateInfo framebuffer_info{};

            framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_info.renderPass = vulkan_type.render_pass;
            framebuffer_info.attachmentCount = 2;
            framebuffer_info.pAttachments = attachments;
            framebuffer_info.width = swapchain.extent.width;
            framebuffer_info.height = swapchain.extent.height;
            framebuffer_info.layers = 1;

            if (vkCreateFramebuffer(vulkan_type.device.handle, &framebuffer_info, nullptr, &swapchain.framebuffers[idx]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }

        return 0;
    }

    int CreateCommandPool()
    {
        QueueFamilyIndices queue_family_indices = FindQueueFamilies(vulkan_type.device.physical_device);

        VkCommandPoolCreateInfo pool_create_info{};
        pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        pool_create_info.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        pool_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

        if (vkCreateCommandPool(vulkan_type.device.handle, &pool_create_info, nullptr, &vulkan_type.command_pool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }

        return 0;
    }

    int CreateCommandBuffer()
    {
        VkCommandBufferAllocateInfo allocate_info{};
        allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocate_info.commandPool = vulkan_type.command_pool;
        allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocate_info.commandBufferCount = 1;

        for (auto& frame : vulkan_type.frame_data)
        {
            if (vkAllocateCommandBuffers(vulkan_type.device.handle, &allocate_info, &frame.command_buffer) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create command buffer");
            }
        }
        return 0;
    }

    void RecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index)
    {
        VkCommandBufferBeginInfo begin_info{};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        begin_info.pInheritanceInfo = nullptr;

        if (vkBeginCommandBuffer(command_buffer, &begin_info) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer");
        }

        //TODO : Depth buffer need to implement

        VkRenderPassBeginInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        render_pass_info.renderPass = vulkan_type.render_pass;
        render_pass_info.framebuffer = vulkan_type.swapchain.framebuffers[image_index];

        render_pass_info.renderArea.offset = { 0,0 };
        render_pass_info.renderArea.extent = vulkan_type.swapchain.extent;

        VkClearValue clear_color[] = {
            {{0.2f,0.3f, 0.1f, 1.0f}},
            {{1.0f, 0.0f}}
        };
        render_pass_info.clearValueCount = 2;
        render_pass_info.pClearValues = clear_color;

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        //vkCmdBindPipeline(command_buffer, &VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_type. )

    }

    int CreateSyncObjects()
    {
        VkSemaphoreCreateInfo semaphore_create_info{};
        semaphore_create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fence_create_info{};
        fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;


        for (auto& frame_data : vulkan_type.frame_data)
        {
            if (vkCreateSemaphore(vulkan_type.device.handle, &semaphore_create_info, nullptr, &frame_data.semaphore.image_available_semaphore) != VK_SUCCESS
                || vkCreateSemaphore(vulkan_type.device.handle, &semaphore_create_info, nullptr, &frame_data.semaphore.render_finished_semaphore) != VK_SUCCESS
                || vkCreateFence(vulkan_type.device.handle, &fence_create_info, nullptr, &frame_data.semaphore.in_flight_fence) != VK_SUCCESS)
            {
                throw std::runtime_error("Failed to create semaphores!");
            }
        }
    }

    int CreateDescriptorPool()
    {


        std::vector<VkDescriptorPoolSize> pool_sizes{
                {VK_DESCRIPTOR_TYPE_SAMPLER , 1000},
    {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1000} ,
    {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,1000} ,
    {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,1000} ,
    {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,1000} ,
    {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,1000} ,
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,1000} ,
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,1000} ,
    {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,1000} ,
    {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,1000} ,
    {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT,1000}
        };


        VkDescriptorPoolCreateInfo descriptor_pool_create_info{};
        descriptor_pool_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        descriptor_pool_create_info.poolSizeCount = pool_sizes.size();
        descriptor_pool_create_info.pPoolSizes = pool_sizes.data();
        descriptor_pool_create_info.maxSets = 1000;

        VK_CHECK(vkCreateDescriptorPool(vulkan_type.device.handle, &descriptor_pool_create_info, nullptr, &vulkan_type.descriptor_pool));

        return 0;
    }

    void CleanupSwapChain()
    {
        for (auto& framebuffer : vulkan_type.swapchain.framebuffers)
        {
            vkDestroyFramebuffer(vulkan_type.device.handle, framebuffer, nullptr);
        }

  /*      for (auto& frame : vulkan_type.frame_data)
        {
            vkFreeCommandBuffers(vulkan_type.device.handle, vulkan_type.command_pool, 1, &frame.command_buffer);
        }*/

        vkDestroyRenderPass(vulkan_type.device.handle, vulkan_type.render_pass, nullptr);

        for (auto image_view : vulkan_type.swapchain.image_views)
        {
            vkDestroyImageView(vulkan_type.device.handle, image_view, nullptr);
        }


    	//TODO :need to implement depth image clear


    	//TODO : need to destory descriptorPool



    	vkDestroySwapchainKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, nullptr);

    }

    int RateDeviceSuitability(VkPhysicalDevice device)
    {
        int score = 0;

        VkPhysicalDeviceProperties physical_properties = {};
        vkGetPhysicalDeviceProperties(device, &physical_properties);
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if (physical_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
        {
            score += 1000;
        }
        score += physical_properties.limits.maxImageDimension2D;

        if (!deviceFeatures.geometryShader)
        {
            return 0;
        }

        return score;
    }



    int CreateVMAallocator()
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
        allocatorInfo.physicalDevice = vulkan_type.device.physical_device;
        allocatorInfo.device = vulkan_type.device.handle;
        allocatorInfo.instance = vulkan_type.instance;
        VK_CHECK(vmaCreateAllocator(&allocatorInfo, &vulkan_type.allocator));

        return 0;
    }


    SwapchainSupportDetails querySwapChainSupport(VkPhysicalDevice device)
    {
        SwapchainSupportDetails details;

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
        const std::vector<const char*> device_extensions =
        {
          VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

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

    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features)
    {
        for (VkFormat format : candidates) {
            VkFormatProperties props;
            vkGetPhysicalDeviceFormatProperties(vulkan_type.device.physical_device, format, &props);

            if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
                return format;
            }
            else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
                return format;
            }
        }

        assert(false, "failed to find supported format!");
    }

    VkFormat FindDepthFormat()
    {
        return FindSupportedFormat(
            { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
            VK_IMAGE_TILING_OPTIMAL,
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
        );
    }

}

