#include "vulkan_context.h"

#define VMA_IMPLEMENTATION
#include "vulkan_type.inl"
#include "vulkan_image.h"
#include "vulkan_shader.h"
#include "vulkan_mesh.h"
#include "vulkan_pipeline.h"
#include "vulkan_material_manager.h"
#include "vulkan_initializers.h"

#include <iostream>
#include <optional>
#include <set>
#include <limits> 

#include "spirv_helper.h"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <algorithm>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

#include "vulkan_material.h"
#include "backends/imgui_impl_vulkan.h"
//#define GLFW_EXPOSE_NATIVE_WIN32s
//#include <GLFW/glfw3native.h>

static VulkanType vulkan_type;

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

    //For Deferred rendering
    void CreateFrameAttachment(VulkanType* vulkan_type, VkFormat format, VkImageUsageFlagBits usage, VulkanFrameBufferAttachment* attachment);
    int buildDeferredCommandBuffer();
    int CreateOffScreenFrameBuffer();
    int SetupDescriptorSet();
    int CreateDeferredShader();
    int CreateDeferredDescriptorSetLayout();
    int CreateDeferredCommandBuffer();
    int CreateDeferredSyncObjects();


    //For Viewport Rendering
    int CreateViewportImage();
    int CreateViewportFramebuffer();
    void ViewportRecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index);


    static VKAPI_ATTR VkBool32 debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT                  messageTypes,
        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
        void* pUserData) {

        //return VK_FALSE;

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

    VulkanContext::VulkanContext(GLFWwindow* window) : Context(window)
    {
        mesh_manager = std::make_unique<VulkanMeshManager>(&vulkan_type);
        material_manager = std::make_unique<VulkanMaterialManager>(&vulkan_type);
        texture_manager = std::make_unique<VulkanTextureManager>(&vulkan_type);
        shader_manager = std::make_unique<VulkanShaderManager>(&vulkan_type);
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
        //deferred
        CreateOffScreenFrameBuffer();
        CreateDeferredSyncObjects();
        CreateDeferredCommandBuffer();
        CreateDeferredDescriptorSetLayout();
        CreateDeferredShader();
        SetupDescriptorSet();

        CreateViewportImage();
        CreateViewportFramebuffer();
        lightpass_set_.Init(&vulkan_type, 2);
        lightpass_set_.AddBindingLayout(0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBindingLayout(1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBindingLayout(2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBindingLayout(3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT)
            .AddBindingLayout(4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT);

        auto& deferred_framebuffer = vulkan_type.deferred_frame_buffer;
        VkDescriptorImageInfo tex_descriptor_position = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.position.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_normal = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.normal.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_albedo = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.albedo.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_emissive = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.emissive.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_metalic_roughness_ao = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.metalic_roughness_ao.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        lightpass_set_.AddBinding(0, &tex_descriptor_position).
            AddBinding(1, &tex_descriptor_normal).
            AddBinding(2, &tex_descriptor_albedo).
            AddBinding(3, &tex_descriptor_emissive).
            AddBinding(4, &tex_descriptor_metalic_roughness_ao);
        lightpass_set_.Build();
        
        //buildDeferredCommandBuffer();
   

        
    }

    // Must be called after vulkan_context.init()
    void VulkanContext::InitGlobalData()
    {
        Context::InitGlobalData();
          
        Vulkan_PipelineBuilder pipeline_builder;

        const uint32_t binding_count = 2;

		global_binding_ubo.resize(binding_count);
        global_binding_ubo[0] = std::make_unique<VulkanUniformBuffer>(&vulkan_type, 0, sizeof(global_data)); // camera data in binding slot 0
        global_binding_ubo[1] = std::make_unique<VulkanUniformBuffer>(&vulkan_type, 1, sizeof(light_data)); // light data in binding slot 1

        global_set.Init(&vulkan_type, 0)
            .AddBindingLayout(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT).AddBinding(0, global_binding_ubo[0].get())
            .AddBindingLayout(1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT).AddBinding(1, global_binding_ubo[1].get())
            .Build();

        //TODO(Austyn): this might go wrong
        vulkan_type.global_pipeline_layout = pipeline_builder.BuildPipeLineLayout(vulkan_type.device.handle, &global_set.layout, 1, 0, 0);
        vulkan_type.current_pipeline_layout = vulkan_type.global_pipeline_layout; 

    }

    void VulkanContext::UpdateGlobalData()
    {
        Context::UpdateGlobalData();

        global_binding_ubo[0]->AddData((void*)&global_data, 0, sizeof(global_data));
        global_binding_ubo[1]->AddData((void*)&light_data, 0, sizeof(light_data));
    }

	// Must be called after init_frame()
    void VulkanContext::BindGlobalData()
    {
        // bind descriptor set here
        global_set.Bind();
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

		// delete global data
        global_set.Cleanup();
        for (auto& global_ubo : global_binding_ubo) {
            global_ubo->Cleanup();
        }

        //TODO(Austyn): Destroy Mesh(buffer), Material(DescriptorSet), Shader, Allocator (vma)
        mesh_manager->Cleanup();
        material_manager->Cleanup();
        shader_manager->Cleanup();
        texture_manager->Cleanup();

        vkDestroyRenderPass(vulkan_type.device.handle, vulkan_type.render_pass, nullptr);

        for (auto image_view : vulkan_type.swapchain.image_views)
        {
            vkDestroyImageView(vulkan_type.device.handle, image_view, nullptr);
        }

        DestroyImage(&vulkan_type, &vulkan_type.swapchain.depth_image);
        vmaDestroyAllocator(vulkan_type.allocator);
        vkDestroySwapchainKHR(vulkan_type.device.handle, vulkan_type.swapchain.handle, nullptr);
        vkDestroyDevice(vulkan_type.device.handle, nullptr);

      
        vkDestroySurfaceKHR(vulkan_type.instance, vulkan_type.surface, 0);
  
        SpirvHelper::Finalize();
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

        if(width == 0 || height == 0)
        {
            return;
        }

  /*      while (width == 0|| height == 0)
        {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }*/

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
        //RecordCommandBuffer(frame_data.command_buffer, frame_data.swap_chain_image_index);
        buildDeferredCommandBuffer();

        while(!start_context_events.empty())
        {
            auto& event = start_context_events.front();
            event();
            start_context_events.pop();
        }

        return 0;
    }

    int VulkanContext::DeferredEndFrame()
    {
        auto& deferred_framebuffer = vulkan_type.deferred_frame_buffer;
        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
        //End offscreen rendering
        vkCmdEndRenderPass(frame_data.command_buffer);
        vkEndCommandBuffer(frame_data.command_buffer);

        auto& semaphore = frame_data.semaphore;
        auto& offscreen_semaphore = vulkan_type.deferred_frame_buffer.offscreenSemaphore;
        VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit.pWaitDstStageMask = &wait_stage;

        //TODO :CHECK IS IT right pWaitsemaphore = image_available_semaphore
        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &semaphore.image_available_semaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores = &offscreen_semaphore;


        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &frame_data.command_buffer;

        vkQueueSubmit(vulkan_type.device.graphics_queue, 1, &submit, frame_data.semaphore.in_flight_fence);
        VK_CHECK(vkWaitForFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence));

        VK_CHECK(vkResetCommandBuffer(frame_data.command_buffer, 0));
        ViewportRecordCommandBuffer(frame_data.command_buffer, frame_data.swap_chain_image_index);
    
        
        //descriptorset write
        VkDescriptorImageInfo tex_descriptor_position = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.position.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_normal = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.normal.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_albedo = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.albedo.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_emissive = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.emissive.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_metalic_roughness_ao = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.metalic_roughness_ao.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);


        lightpass_set_.AddBinding(0, &tex_descriptor_position).
    	AddBinding(1,&tex_descriptor_normal).
    	AddBinding(2,&tex_descriptor_albedo).
    	AddBinding(3,&tex_descriptor_emissive).
    	AddBinding(4,&tex_descriptor_metalic_roughness_ao);

        
        shader_manager->GetShader("shader_lightpass")->BindDeferred();
        lightpass_set_.Bind();
        global_set.Bind();

        // draw quad

        vkCmdDraw(frame_data.command_buffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(frame_data.command_buffer);
        vkEndCommandBuffer(frame_data.command_buffer);

        submit.pWaitDstStageMask = &wait_stage;

        //TODO :CHECK IS IT right pWaitsemaphore = image_available_semaphore
        submit.waitSemaphoreCount = 1;
        submit.signalSemaphoreCount = 1;
        submit.commandBufferCount = 1;

        submit.waitSemaphoreCount = 1;
        submit.pWaitSemaphores = &offscreen_semaphore;
        //submit.pWaitSemaphores = &semaphore.image_available_semaphore;

        submit.signalSemaphoreCount = 1;
        submit.pSignalSemaphores =  & vulkan_type.viewport_frame_buffer.viewport_semaphore;


        submit.commandBufferCount = 1;
        submit.pCommandBuffers = &frame_data.command_buffer;
        vkQueueSubmit(vulkan_type.device.graphics_queue, 1, &submit, frame_data.semaphore.in_flight_fence);
        VK_CHECK(vkWaitForFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence, VK_TRUE, UINT64_MAX));
        VK_CHECK(vkResetFences(vulkan_type.device.handle, 1, &frame_data.semaphore.in_flight_fence));

        VK_CHECK(vkResetCommandBuffer(frame_data.command_buffer, 0));
        RecordCommandBuffer(frame_data.command_buffer, frame_data.swap_chain_image_index);

        //ImGui::Begin("viewport");
        //const auto* TextureID = dynamic_cast<Renderer::VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.GetImGuiTextureID();
        //UpdateViewportDescriptorSet(*TextureID, 0);
        //ImGui::Image(*TextureID, ImVec2(800, 600));
        //ImGui::End();

        while (!end_deferred_endframe_events.empty())
        {
            auto& event = end_deferred_endframe_events.front();
            event();
            end_deferred_endframe_events.pop();
        }

        return 0;
    }

    int VulkanContext::EndFrame()
    {
        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];

        //before
        //auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
        //vkCmdEndRenderPass(frame_data.command_buffer);
        //vkEndCommandBuffer(frame_data.command_buffer);

        //auto& semaphore = frame_data.semaphore;
        //VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

        //VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        //submit.pWaitDstStageMask = &wait_stage;

        ////TODO :CHECK IS IT right pWaitsemaphore = image_available_semaphore
        //submit.waitSemaphoreCount = 1;
        //submit.pWaitSemaphores = &semaphore.image_available_semaphore;

        //submit.signalSemaphoreCount = 1;
        //submit.pSignalSemaphores = &semaphore.render_finished_semaphore;

        //submit.commandBufferCount = 1;
        //submit.pCommandBuffers = &frame_data.command_buffer;
        while (!end_context_events.empty())
        {
            auto& event = end_context_events.front();
            event();
            end_context_events.pop();
        }

        ///
        auto& semaphore = frame_data.semaphore;
        auto& offscreen_semaphore = vulkan_type.deferred_frame_buffer.offscreenSemaphore;
        VkSubmitInfo submit{ VK_STRUCTURE_TYPE_SUBMIT_INFO };

        VkPipelineStageFlags wait_stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

        submit.pWaitDstStageMask = &wait_stage;

        //TODO :CHECK IS IT right pWaitsemaphore = image_available_semaphore
        submit.waitSemaphoreCount = 1;
        submit.signalSemaphoreCount = 1;
    	submit.commandBufferCount = 1;


        vkCmdEndRenderPass(frame_data.command_buffer);
        vkEndCommandBuffer(frame_data.command_buffer);

        submit.pWaitSemaphores = &vulkan_type.viewport_frame_buffer.viewport_semaphore;
        submit.pSignalSemaphores = &semaphore.render_finished_semaphore;

        submit.pCommandBuffers = &frame_data.command_buffer;
        vkQueueSubmit(vulkan_type.device.graphics_queue, 1, &submit, frame_data.semaphore.in_flight_fence);
        ///

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

        dynamic_cast<VulkanTextureManager*>(texture_manager.get())->vulkan_texture_imgui_descriptor_pool.ResetCount();
        return  0;
    }

    void VulkanContext::UpdateViewportDescriptorSet(VkDescriptorSet descriptor_set, int dest_binding)
    {
        VkDescriptorImageInfo image_buffer_info
        {
                .sampler = vulkan_type.viewport_frame_buffer.color_sampler,
                .imageView = vulkan_type.viewport_frame_buffer.viewport_vulkan_images[vulkan_type.current_frame].image_view,
                .imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL
        };

        VkWriteDescriptorSet descriptor_set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
        descriptor_set_write.dstSet = descriptor_set;
        descriptor_set_write.dstBinding = 0;
        descriptor_set_write.dstArrayElement = 0;
        descriptor_set_write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptor_set_write.descriptorCount = 1;
        descriptor_set_write.pImageInfo = &image_buffer_info;

        vkUpdateDescriptorSets(vulkan_type.device.handle, 1, &descriptor_set_write, 0, nullptr);
    }

    VulkanType* VulkanContext::GetVulkanType()
    {
        return &vulkan_type;
    }
 
    void VulkanContext::DrawQueue()
    {
	    Context::DrawQueue();

        if (!draw_queue.empty())
            while (!draw_queue.empty())
            {
                const auto& front = draw_queue.front();
                  
                auto* transform = front.transform;
                auto* mesh = front.mesh;
                auto* shader = front.shader;
                auto* material = front.material;

                //TODO : Draw call
                //glm::mat4 model(1.f);
                //model = glm::translate(model, transform->position);
                //model = glm::scale(model, transform->scale);

                //glm::quat rotP = glm::angleAxis(glm::radians(transform->rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
                //glm::quat rotY = glm::angleAxis(glm::radians(transform->rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
                //glm::quat rotR = glm::angleAxis(glm::radians(transform->rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

                //model = model * glm::mat4_cast(rotR) * glm::mat4_cast(rotY) * glm::mat4_cast(rotP);
                glm::mat4 model = transform->GetMatrix();

                glm::mat3 normal_matrix = glm::transpose(glm::inverse(model));


                //TODO : need to change deferred offscreen shader
                /*if (shader_manager->GetShader(front.shader->name)->reload_next_frame)
                    shader_manager->GetShader(front.shader->name)->Reload();

                shader_manager->GetShader(front.shader->name)->Bind();*/ // Bind pipeline & descriptor set 1

                vulkan_type.deferred_frame_buffer.deferred_shader->Bind();
				BindGlobalData();

            	//TODO: Maybe later, material update should be in update and sorted function
                  
                /*
                if (!material->is_light)
                {

                        if (auto* iter = material_manager->GetMaterial(material->name); iter != nullptr)
                        {
                            iter->Bind();
                        }
	                    
           
                }
                else
                {
                    light_material->InitMaterialData(std::move(material->data));
                    light_material->is_changed = true;
                    light_material->Bind();
                }
                */
               if(auto* find = material_manager->GetMaterial(material->name); find != nullptr )
               {
                   find->Bind();
               }

                //TODO: Bind Object Descriptor set 3 in future
                if (mesh->mesh_name.size() != 0) {
                    mesh_manager->DrawMesh(mesh->mesh_name.c_str(), model, normal_matrix); // Draw Object
                }
                draw_queue.pop();
            }
      
    }

    void VulkanContext::AddStartContextEvent(const EventType& f)
    {
        start_context_events.push(f);
    }

    void VulkanContext::AddAfterEndDeferredEvent(const EventType& f)
    {
        end_deferred_endframe_events.push(f);
    }

    void VulkanContext::AddEndContextEvent(const EventType& f)
    {
        end_context_events.push(f);
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
        //debugCreateInfo.pfnUserCallback = (PFN_vkDebugUtilsMessengerCallbackEXT)debugCallback;
        debugCreateInfo.pUserData = 0;
        ;
        //vkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger);


        //because of the static library
        PFN_vkCreateDebugUtilsMessengerEXT myvkCreateDebugUtilsMessengerEXT = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(vulkan_type.instance, "vkCreateDebugUtilsMessengerEXT"));


        //VK_CHECK(myvkCreateDebugUtilsMessengerEXT(vulkan_type.instance, &debugCreateInfo, nullptr, &vulkan_type.debug_messenger));
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

        vkGetPhysicalDeviceProperties(vulkan_type.device.physical_device, &vulkan_type.device.properties);

        return 0;
    }


    bool IsDevicesSuitable(VkPhysicalDevice device)
    {
        VkPhysicalDeviceProperties device_property;
        vkGetPhysicalDeviceProperties(device, &device_property);

        //if (device_property.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        //    return false;
        //}

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
            VK_IMAGE_ASPECT_DEPTH_BIT,1);

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

        VkClearValue clear_color = {
            {{0.01f,0.01f, 0.01f, 1.f}}
        };

        VkClearValue depth_clear;
        depth_clear.depthStencil.depth = 1.f;
        depth_clear.depthStencil.stencil = 0;
        VkClearValue clear_values[] = {clear_color, depth_clear};

        render_pass_info.clearValueCount = 2;
        render_pass_info.pClearValues = clear_values;

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
    	VkViewport viewport = VulkanInitializer::ViewPort(1600, 900, 0.f, 1.f);
        vkCmdSetViewport(frame_data.command_buffer, 0, 1, &viewport);

        VkRect2D scissor = VulkanInitializer::Rect2D(1600, 900, 0, 0);

        vkCmdSetScissor(frame_data.command_buffer, 0, 1, &scissor);

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

        DestroyImage(&vulkan_type, &vulkan_type.swapchain.depth_image);

        for (auto image_view : vulkan_type.swapchain.image_views)
        {
            vkDestroyImageView(vulkan_type.device.handle, image_view, nullptr);
        }

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

    //deferred rendering
    void CreateFrameAttachment(VulkanType* vulkan_type, VkFormat format, VkImageUsageFlagBits usage, VulkanFrameBufferAttachment* attachment)
    {
        VkImageAspectFlags aspect_mask = 0;
        VkImageLayout image_layout;

        attachment->format = format;
        if(usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
        {
            aspect_mask = VK_IMAGE_ASPECT_COLOR_BIT;
            image_layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }
        if(usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
        {
            aspect_mask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            image_layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        }

        if(aspect_mask < 0)
        {
            std::runtime_error("Error frame buffer attachment aspect mask error!");
        }

        CreateImage(vulkan_type, attachment, VK_IMAGE_TYPE_2D, vulkan_type->swapchain.extent.width, vulkan_type->swapchain.extent.height, attachment->format, usage | VK_IMAGE_USAGE_SAMPLED_BIT, VK_IMAGE_TILING_OPTIMAL, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true, aspect_mask,1);
    }

    //first rendering
    int buildDeferredCommandBuffer()
    {
         auto& deferred_frame_buffer = vulkan_type.deferred_frame_buffer;
         auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];



        VkCommandBufferBeginInfo command_buffer_begin_info = VulkanInitializer::CommandBufferBeginInfo();
        std::array<VkClearValue, 6> clear_values;
        clear_values[0].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[1].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[2].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[3].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[4].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };
        clear_values[5].depthStencil = { 1.0f, 0 };


 
        VkRenderPassBeginInfo render_pass_begin_info = VulkanInitializer::RenderPassBeginInfo();
        render_pass_begin_info.renderPass  = deferred_frame_buffer.render_pass;
        render_pass_begin_info.framebuffer = deferred_frame_buffer.frame_buffer;
        render_pass_begin_info.renderArea.extent.width = deferred_frame_buffer.width;
        render_pass_begin_info.renderArea.extent.height = deferred_frame_buffer.height;
    	render_pass_begin_info.clearValueCount = static_cast<uint32_t>(clear_values.size());
        render_pass_begin_info.pClearValues = clear_values.data();



        VK_CHECK(vkBeginCommandBuffer(frame_data.command_buffer, &command_buffer_begin_info));

        vkCmdBeginRenderPass(frame_data.command_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);


        VkViewport viewport = VulkanInitializer::ViewPort(1600, 900, 0.f, 1.f);
        vkCmdSetViewport(frame_data.command_buffer, 0, 1, &viewport);

        VkRect2D scissor = VulkanInitializer::Rect2D(1600, 900, 0, 0);

        vkCmdSetScissor(frame_data.command_buffer, 0, 1, &scissor);



        return 0;

    }

    int CreateOffScreenFrameBuffer()
    {
        //TODO : Chanage magic numbers
        vulkan_type.deferred_frame_buffer.width = 1600;
        vulkan_type.deferred_frame_buffer.height = 900;

        CreateFrameAttachment(&vulkan_type, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.position);
        CreateFrameAttachment(&vulkan_type, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.normal);
        CreateFrameAttachment(&vulkan_type, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.albedo);
        CreateFrameAttachment(&vulkan_type, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.emissive);
        CreateFrameAttachment(&vulkan_type, VK_FORMAT_R16G16B16A16_SFLOAT, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.metalic_roughness_ao);

     
        VkFormat attachment_depth_format;
        VkBool32 valid_depth_format = VulkanInitializer::GetSupportedDepthFormat(vulkan_type.device.physical_device, &attachment_depth_format);
        assert(valid_depth_format);

        CreateFrameAttachment(&vulkan_type, attachment_depth_format, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, &vulkan_type.deferred_frame_buffer.depth);

    	std::array<VkAttachmentDescription, 6> attachment_descriptions = {};

        for(uint32_t idx = 0; idx < 6; ++idx)
        {
            attachment_descriptions[idx].samples = VK_SAMPLE_COUNT_1_BIT;
            attachment_descriptions[idx].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
            attachment_descriptions[idx].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            attachment_descriptions[idx].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
            attachment_descriptions[idx].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

            if(idx == 5)
            {
                attachment_descriptions[idx].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descriptions[idx].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
            }
            else
            {
                attachment_descriptions[idx].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                attachment_descriptions[idx].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            }
        }

        attachment_descriptions[0].format = vulkan_type.deferred_frame_buffer.position.format;
        attachment_descriptions[1].format = vulkan_type.deferred_frame_buffer.normal.format;
        attachment_descriptions[2].format = vulkan_type.deferred_frame_buffer.albedo.format;
        attachment_descriptions[3].format = vulkan_type.deferred_frame_buffer.emissive.format;
        attachment_descriptions[4].format = vulkan_type.deferred_frame_buffer.metalic_roughness_ao.format;
        attachment_descriptions[5].format = vulkan_type.deferred_frame_buffer.depth.format;

        std::vector<VkAttachmentReference> color_references;

        for(uint32_t idx =0; idx < 5; idx++)
        {
            color_references.push_back({idx, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL});
        }

        // depth attachment
        VkAttachmentReference depth_reference{5,VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL};

        VkSubpassDescription subpass{};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.pColorAttachments = color_references.data();
        subpass.colorAttachmentCount = static_cast<uint32_t>(color_references.size());
        subpass.pDepthStencilAttachment = &depth_reference;

        std::array<VkSubpassDependency, 2> dependencies;

        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.pAttachments = attachment_descriptions.data();
        render_pass_info.attachmentCount = static_cast<uint32_t>(attachment_descriptions.size());
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass;
        render_pass_info.dependencyCount = 2;
        render_pass_info.pDependencies = dependencies.data();

        VK_CHECK(vkCreateRenderPass(vulkan_type.device.handle, &render_pass_info, nullptr, &vulkan_type.deferred_frame_buffer.render_pass));

        std::array<VkImageView, 6> attachments;

        auto& deferred_framebuffer = vulkan_type.deferred_frame_buffer;

        attachments[0] = deferred_framebuffer.position.image_view;
        attachments[1] = deferred_framebuffer.normal.image_view;
        attachments[2] = deferred_framebuffer.albedo.image_view;
        attachments[3] = deferred_framebuffer.emissive.image_view;
        attachments[4] = deferred_framebuffer.metalic_roughness_ao.image_view;
        attachments[5] = deferred_framebuffer.depth.image_view;

        VkFramebufferCreateInfo framebuffer_create_info{};
        framebuffer_create_info.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_create_info.pNext           = NULL;
        framebuffer_create_info.renderPass      = deferred_framebuffer.render_pass;
        framebuffer_create_info.pAttachments    = attachments.data();
        framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_create_info.width           = deferred_framebuffer.width;
        framebuffer_create_info.height          = deferred_framebuffer.height;
        framebuffer_create_info.layers          = 1;

        VK_CHECK(vkCreateFramebuffer(vulkan_type.device.handle, &framebuffer_create_info, nullptr, &deferred_framebuffer.frame_buffer));

        VkSamplerCreateInfo sampler_create_info = VulkanInitializer::SamplerCreateInfo();
        sampler_create_info.magFilter = VK_FILTER_NEAREST;
        sampler_create_info.minFilter = VK_FILTER_NEAREST;
        sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_create_info.addressModeV = sampler_create_info.addressModeU;
        sampler_create_info.addressModeW = sampler_create_info.addressModeU;
        sampler_create_info.mipLodBias = 0.f;
        sampler_create_info.maxAnisotropy = 1.f;
        sampler_create_info.minLod = 0.f;
        sampler_create_info.maxLod = 1.f;
        sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        VK_CHECK(vkCreateSampler(vulkan_type.device.handle, &sampler_create_info, nullptr, &deferred_framebuffer.color_sampler));



        return 0;
    }

    int SetupDescriptorSet()
    {
        std::vector<VkWriteDescriptorSet> write_descriptor_sets;
        VkDescriptorSetAllocateInfo allocate_info = VulkanInitializer::DescriptorSetAllocateInfo(vulkan_type.descriptor_pool, &vulkan_type.deferred_frame_buffer.layout,1);

    	auto& deferred_framebuffer = vulkan_type.deferred_frame_buffer;

        VkDescriptorImageInfo tex_descriptor_position             = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.position.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_normal               = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.normal.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_albedo               = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.albedo.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_emissive             = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.emissive.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
        VkDescriptorImageInfo tex_descriptor_metalic_roughness_ao = VulkanInitializer::DescriptorImageInfo(deferred_framebuffer.color_sampler, deferred_framebuffer.metalic_roughness_ao.image_view, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

        //TODO : DO I NEED TO ALLOCATE ONETIME?
        VK_CHECK(vkAllocateDescriptorSets(vulkan_type.device.handle, &allocate_info, &vulkan_type.deferred_frame_buffer.descriptor_set));
        //Deferred
        write_descriptor_sets =
        {
            VulkanInitializer::WriteDescriptorSet(deferred_framebuffer.descriptor_set,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,1, &tex_descriptor_position),
            VulkanInitializer::WriteDescriptorSet(deferred_framebuffer.descriptor_set,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,2, &tex_descriptor_normal),
            VulkanInitializer::WriteDescriptorSet(deferred_framebuffer.descriptor_set,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,3, &tex_descriptor_albedo),
            VulkanInitializer::WriteDescriptorSet(deferred_framebuffer.descriptor_set,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,4, &tex_descriptor_emissive),
            VulkanInitializer::WriteDescriptorSet(deferred_framebuffer.descriptor_set,VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 5, &tex_descriptor_metalic_roughness_ao),
        };


        vkUpdateDescriptorSets(vulkan_type.device.handle, static_cast<uint32_t>(write_descriptor_sets.size()), write_descriptor_sets.data(), 0, nullptr);


    	return 0;
    }

    int CreateDeferredShader()
    {
        ShaderConfig shader_config = { "shader_geometrypass", {Renderer::E_StageType::VERTEX_SHADER,
                        Renderer::E_StageType::FRAGMENT_SHADER }, 2 };
         

        auto& shader = vulkan_type.deferred_frame_buffer.deferred_shader;
        shader = std::make_shared<VulkanShader>(&vulkan_type);
        shader->Init(&shader_config);

        return 0;
         
	}


    int CreateDeferredDescriptorSetLayout()
    {
        std::vector<VkDescriptorSetLayoutBinding> set_layout_bindings =
        {
			VulkanInitializer::DescripterSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT,1),
        	VulkanInitializer::DescripterSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,2),
            VulkanInitializer::DescripterSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,3),
            VulkanInitializer::DescripterSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,4),
            VulkanInitializer::DescripterSetLayoutBinding(VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,VK_SHADER_STAGE_FRAGMENT_BIT,5),
        };

        VkDescriptorSetLayoutCreateInfo  descriptor_layout = VulkanInitializer::DescriptorSetLayoutCreateInfo(set_layout_bindings);
        VK_CHECK(vkCreateDescriptorSetLayout(vulkan_type.device.handle, &descriptor_layout, nullptr, &vulkan_type.deferred_frame_buffer.layout));

       /* VkPipelineLayoutCreateInfo pipeline_layout_create_info = VulkanInitializer::pipelineLayoutCreateInfo(&vulkan_type.deferred_frame_buffer.layout, 1);
        VK_CHECK(vkCreatePipelineLayout(vulkan_type.device.handle, &pipeline_layout_create_info, nullptr, &vulkan_type.deferred_frame_buffer.deferred_shader->pipeline_layout));*/

        return 0;
    }

    int CreateDeferredCommandBuffer()
    {
        VkCommandBufferAllocateInfo vk_command_buffer_allocate_info{};

        vk_command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        vk_command_buffer_allocate_info.commandPool = vulkan_type.command_pool;
        vk_command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        vk_command_buffer_allocate_info.commandBufferCount = 1;

        VK_CHECK(vkAllocateCommandBuffers(vulkan_type.device.handle, &vk_command_buffer_allocate_info, &vulkan_type.deferred_frame_buffer.off_screen_command_buffer));
        return 0;
    }

    int CreateDeferredSyncObjects()
    {
        VkSemaphoreCreateInfo semaphore_create_info = VulkanInitializer::SemaphoreCreateInfo();
        VK_CHECK(vkCreateSemaphore(vulkan_type.device.handle, &semaphore_create_info, nullptr, &vulkan_type.deferred_frame_buffer.offscreenSemaphore));
        VK_CHECK(vkCreateSemaphore(vulkan_type.device.handle, &semaphore_create_info, nullptr, &vulkan_type.viewport_frame_buffer.viewport_semaphore));
        return 0;
    }

    int CreateViewportImage()
    {
        auto& viewport_framebuffer = vulkan_type.viewport_frame_buffer;
        auto& swapchain = vulkan_type.swapchain;

        const uint32_t size = vulkan_type.swapchain.images.size();
        vulkan_type.viewport_frame_buffer.viewport_vulkan_images.resize(size);

        for(uint32_t idx = 0; idx < size; idx++)
        {
            CreateFrameAttachment(&vulkan_type, VK_FORMAT_B8G8R8A8_SRGB, VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, &viewport_framebuffer.viewport_vulkan_images[idx]);
        }
        return 0;
    }

    int CreateViewportFramebuffer()
    {
        auto& viewport_framebuffer = vulkan_type.viewport_frame_buffer;

        const uint32_t size = viewport_framebuffer.viewport_vulkan_images.size();
        viewport_framebuffer.frame_buffers.resize(size);

        VkAttachmentDescription color_attachment{};
        color_attachment.format = vulkan_type.swapchain.image_format;
        color_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

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


    	if (vkCreateRenderPass(vulkan_type.device.handle, &render_pass_info, nullptr, &viewport_framebuffer.render_pass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }

        for(int idx = 0; idx < size; ++idx)
        {
            std::array<VkImageView, 2> attachments = {
                    viewport_framebuffer.viewport_vulkan_images[idx].image_view,
                    vulkan_type.swapchain.depth_image.image_view};

            VkFramebufferCreateInfo framebuffer_create_info{};
            framebuffer_create_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebuffer_create_info.renderPass = viewport_framebuffer.render_pass;
            framebuffer_create_info.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebuffer_create_info.pAttachments = attachments.data();
            framebuffer_create_info.width = vulkan_type.swapchain.extent.width;
            framebuffer_create_info.height = vulkan_type.swapchain.extent.height;
            framebuffer_create_info.layers = 1;


            VK_CHECK(vkCreateFramebuffer(vulkan_type.device.handle, &framebuffer_create_info, nullptr, &viewport_framebuffer.frame_buffers[idx]));
        }

        VkSamplerCreateInfo sampler_create_info = VulkanInitializer::SamplerCreateInfo();
        sampler_create_info.magFilter = VK_FILTER_NEAREST;
        sampler_create_info.minFilter = VK_FILTER_NEAREST;
        sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        sampler_create_info.addressModeV = sampler_create_info.addressModeU;
        sampler_create_info.addressModeW = sampler_create_info.addressModeU;
        sampler_create_info.mipLodBias = 0.f;
        sampler_create_info.maxAnisotropy = 1.f;
        sampler_create_info.minLod = 0.f;
        sampler_create_info.maxLod = 1.f;
        sampler_create_info.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;

        VK_CHECK(vkCreateSampler(vulkan_type.device.handle, &sampler_create_info, nullptr, &viewport_framebuffer.color_sampler));

    }

    void ViewportRecordCommandBuffer(VkCommandBuffer command_buffer, uint32_t image_index)
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
        render_pass_info.renderPass = vulkan_type.viewport_frame_buffer.render_pass;
        render_pass_info.framebuffer = vulkan_type.viewport_frame_buffer.frame_buffers[image_index];

        render_pass_info.renderArea.offset = { 0,0 };
        render_pass_info.renderArea.extent = vulkan_type.swapchain.extent;

        VkClearValue clear_color = {
            {{0.01f,0.01f, 0.01f, 1.f}}
        };

        VkClearValue depth_clear;
        depth_clear.depthStencil.depth = 1.f;
        depth_clear.depthStencil.stencil = 0;
        VkClearValue clear_values[] = { clear_color, depth_clear };

        render_pass_info.clearValueCount = 2;
        render_pass_info.pClearValues = clear_values;

        vkCmdBeginRenderPass(command_buffer, &render_pass_info, VK_SUBPASS_CONTENTS_INLINE);

        auto& frame_data = vulkan_type.frame_data[vulkan_type.current_frame];
        VkViewport viewport = VulkanInitializer::ViewPort(1600, 900, 0.f, 1.f);
        vkCmdSetViewport(frame_data.command_buffer, 0, 1, &viewport);

        VkRect2D scissor = VulkanInitializer::Rect2D(1600, 900, 0, 0);

        vkCmdSetScissor(frame_data.command_buffer, 0, 1, &scissor);


    }

}

