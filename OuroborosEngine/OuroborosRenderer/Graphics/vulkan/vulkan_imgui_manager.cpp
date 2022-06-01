#include "vulkan_imgui_manager.h"

#include <stdexcept>
#include "imgui-docking/imgui.h"
#include "imgui-docking/backends/imgui_impl_glfw.h"
#include "imgui-docking/backends/imgui_impl_vulkan.h"

#include "vulkan_type.inl"

namespace Renderer
{
	VulkanImguiManager::VulkanImguiManager() : ImguiManager()
	{


	}


	void VulkanImguiManager::Init(GLFWwindow* window)
	{
		window_ = window;
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		//ImGui::StyleColorsDark();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		ImGuiStyle& style = ImGui::GetStyle();

		bool bStyleDark_ = true;
		float alpha_ = 0.8;

		style.Alpha = 1.0f;
		style.FrameRounding = 3.0f;
		style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
		style.Colors[ImGuiCol_WindowBg] = ImVec4(0.94f, 0.94f, 0.94f, 0.94f);
		//style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		style.Colors[ImGuiCol_PopupBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 0.00f, 0.00f, 0.39f);
		style.Colors[ImGuiCol_BorderShadow] = ImVec4(1.00f, 1.00f, 1.00f, 0.10f);
		style.Colors[ImGuiCol_FrameBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.94f);
		style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		style.Colors[ImGuiCol_TitleBg] = ImVec4(0.96f, 0.96f, 0.96f, 1.00f);
		style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 1.00f, 1.00f, 0.51f);
		style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
		style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.98f, 0.98f, 0.98f, 0.53f);
		style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
		style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.49f, 0.49f, 0.49f, 1.00f);
		//style.Colors[ImGuiCol_ComboBg] = ImVec4(0.86f, 0.86f, 0.86f, 0.99f);
		style.Colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.52f, 0.88f, 1.00f);
		style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Button] = ImVec4(0.26f, 0.59f, 0.98f, 0.40f);
		style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_Header] = ImVec4(0.26f, 0.59f, 0.98f, 0.31f);
		style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
		style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		//style.Colors[ImGuiCol_Column] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		//style.Colors[ImGuiCol_ColumnHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.78f);
		//style.Colors[ImGuiCol_ColumnActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
		style.Colors[ImGuiCol_ResizeGrip] = ImVec4(1.00f, 1.00f, 1.00f, 0.50f);
		style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
		style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
		//style.Colors[ImGuiCol_CloseButton] = ImVec4(0.59f, 0.59f, 0.59f, 0.50f);
		//style.Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		//style.Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.98f, 0.39f, 0.36f, 1.00f);
		style.Colors[ImGuiCol_PlotLines] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
		style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
		style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
		//style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);

		if (bStyleDark_)
		{
			for (int i = 0; i <= ImGuiCol_COUNT; i++)
			{
				ImVec4& col = style.Colors[i];
				float H, S, V;
				ImGui::ColorConvertRGBtoHSV(col.x, col.y, col.z, H, S, V);

				if (S < 0.1f)
				{
					V = 1.0f - V;
				}
				ImGui::ColorConvertHSVtoRGB(H, S, V, col.x, col.y, col.z);
				if (col.w < 1.00f)
				{
					col.w *= alpha_;
				}
			}
		}
		else
		{
			for (int i = 0; i <= ImGuiCol_COUNT; i++)
			{
				ImVec4& col = style.Colors[i];
				if (col.w < 1.00f)
				{
					col.x *= alpha_;
					col.y *= alpha_;
					col.z *= alpha_;
					col.w *= alpha_;
				}
			}
		}



		VkDescriptorPoolSize pool_sizes[] =
		{
			{ VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
			{ VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
			{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
			{ VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
		};

		VkDescriptorPoolCreateInfo pool_info = {};
		pool_info.sType			= VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		pool_info.flags			= VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		pool_info.maxSets		= 1000;
		pool_info.poolSizeCount = std::size(pool_sizes);
		pool_info.pPoolSizes	= pool_sizes;

		VkDescriptorPool imguiPool;
		VK_CHECK(vkCreateDescriptorPool(vulkan_type->device.handle, &pool_info, nullptr, &imguiPool));


		ImGui_ImplGlfw_InitForVulkan(window, true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		init_info.Instance			= vulkan_type->instance;
		init_info.PhysicalDevice	= vulkan_type->device.physical_device;
		init_info.Device			= vulkan_type->device.handle;
		init_info.Queue				= vulkan_type->device.present_queue;
		init_info.DescriptorPool	= imguiPool;
		init_info.MinImageCount		= 3;
		init_info.ImageCount		= 3;

		VkAttachmentDescription attachment = {};
		attachment.format			= vulkan_type->swapchain.image_format;
		attachment.samples			= VK_SAMPLE_COUNT_1_BIT;
		attachment.loadOp			= VK_ATTACHMENT_LOAD_OP_LOAD;
		attachment.storeOp			= VK_ATTACHMENT_STORE_OP_STORE;
		attachment.stencilLoadOp	= VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachment.stencilStoreOp   = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachment.initialLayout	= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		attachment.finalLayout		= VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;


		VkAttachmentReference color_attachment = {};
		color_attachment.attachment = 0;
		color_attachment.layout		= VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments	 = &color_attachment;

		VkSubpassDependency dependency = {};
		dependency.srcSubpass		 = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass		 = 0;
		dependency.srcStageMask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask		 = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask	 = 0;  // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dstAccessMask	 = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo info = {};
		info.sType			 = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		info.attachmentCount = 1;
		info.pAttachments	 = &attachment;
		info.subpassCount	 = 1;
		info.pSubpasses		 = &subpass;
		info.dependencyCount = 1;
		info.pDependencies	 = &dependency;

		/*if (vkCreateRenderPass(vulkan_type->device.handle, &info, nullptr, &imgui_render_pass) != VK_SUCCESS)
		{
			throw std::runtime_error("Could not create Dear ImGui's render pass");
		}*/

		ImGui_ImplVulkan_Init(&init_info, vulkan_type->render_pass);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType				 = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level				 = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool		 = vulkan_type->command_pool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(vulkan_type->device.handle, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType			  = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &commandBuffer;

		vkQueueSubmit(vulkan_type->device.graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(vulkan_type->device.graphics_queue);

		vkFreeCommandBuffers(vulkan_type->device.handle, vulkan_type->command_pool, 1, &commandBuffer);
		ImGui_ImplVulkan_DestroyFontUploadObjects();

	}

	void VulkanImguiManager::VulkanInit(Vulkan_type* vulkan_type_)
	{
		vulkan_type = vulkan_type_;
	}

	void VulkanImguiManager::BeginFrame()
	{
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void VulkanImguiManager::EndFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGui::Render();
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), vulkan_type->frame_data[vulkan_type->current_frame].command_buffer);
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) 
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	

	}

	void VulkanImguiManager::Shutdown()
	{
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void VulkanImguiManager::Update()
	{
		ImguiManager::Update();
		ImGui::ShowDemoWindow();
	}

}
