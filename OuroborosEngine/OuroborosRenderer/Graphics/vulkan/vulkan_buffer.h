#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include "../buffer.h"
#include <vulkan_core.h>
#include "../mesh.h"
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Renderer
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(VmaAllocator* allocator, uint64_t buffer_size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage vma_usage);
		~VulkanBuffer();
		void UploadData(void* data, uint64_t buffer_size);
		static void CopyBuffer(VkQueue* queue, VulkanBuffer* src_buffer, VulkanBuffer* dst_buffer, VkDeviceSize size);
		static void CopyBufferToImage(VkCommandBuffer cmd, VkImageLayout dst_image_layout, VulkanBuffer* src_buffer, VkImage* dst_image, VkExtent3D image_extent);

		VkBuffer buffer = nullptr;
		VmaAllocation allocation = nullptr;
	private:
		VmaAllocator* allocator = nullptr;

	};


	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer() = default;
		~VulkanVertexBuffer() override;
		void Bind() const override;
		void UnBind() const override {};
		void AddData(const std::vector<Vertex>& vertices);
		uint64_t GetCount() { return count; }

	private:
		std::shared_ptr<VulkanBuffer> buffer;

		uint64_t count = 0;
		uint64_t buffer_size;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer() =default;
		~VulkanIndexBuffer() override;
		void Bind() const override;
		void UnBind() const override;

		void AddData(const std::vector<uint32_t>& data);
		uint64_t GetCount() { return count; }
	private:
		uint64_t buffer_size;
		std::shared_ptr<VulkanBuffer> buffer;
		uint64_t count = 0;
	};

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer() = default;
		~VulkanUniformBuffer() override;
		void Bind() const override;
		void UnBind() const override;
	private:
		VkDescriptorSet descriptor_set{ VK_NULL_HANDLE };
		uint64_t buffer_size;
		std::shared_ptr<VulkanBuffer> buffer;

	};

}



#endif //!VULKAN_BUFFER_H
