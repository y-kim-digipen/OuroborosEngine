#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#include "vulkan_material.h"
#include "vulkan_texture_manager.h"
#include "../context.h"

struct VulkanType;

namespace Renderer {

	using EventType = std::function<void(void)>;

	class VulkanContext : public Context {
	public:
		VulkanContext(GLFWwindow* window);
		void Init(int major, int minor) override;

		void InitGlobalData() override;
		void UpdateGlobalData() override;
		void BindGlobalData() override;

		void Shutdown() override;

		int BeginFrame() override;
		int DeferredEndFrame() override;
		int EndFrame() override;


		void UpdateViewportDescriptorSet(VkDescriptorSet descriptor_set, int dest_binding);

		std::unique_ptr<VulkanMeshManager> mesh_manager;

		VulkanType* GetVulkanType();

		void DrawQueue() override;

		void AddStartContextEvent(const EventType& f);
		void AddAfterEndDeferredEvent(const EventType& f);
		void AddEndContextEvent(const EventType& f);

		static void ChangeSceneScreenSize(uint16_t width, uint16_t height);
		static std::pair<uint16_t, uint16_t> GetSceneScreenSize();
	private:
		void CreateSurface();
		void CreateSwapChain();
		void RecreateSwapChain();
		DescriptorSet lightpass_set_;

		std::queue<EventType> start_context_events;
		std::queue<EventType> end_deferred_endframe_events;
		std::queue<EventType> end_context_events;

		inline static uint16_t scene_screen_size_width;
		inline static uint16_t scene_screen_size_height;
	};

}

#endif // !VULKAN_CONTEXT_H
