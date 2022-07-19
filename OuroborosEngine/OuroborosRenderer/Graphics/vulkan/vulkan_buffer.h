#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include <memory>

#include "vulkan_type.inl"

#include "../buffer.h"
#include "../mesh.h"

namespace Renderer
{
	class VulkanBuffer
	{
	public:
		VulkanBuffer(Vulkan_type* vulkan_type, uint64_t buffer_size, VkBufferUsageFlags buffer_usage, VmaMemoryUsage vma_usage);
		~VulkanBuffer();
		void UploadData(void* data, uint64_t buffer_size, uint32_t offset = 0);
		void CopyBuffer(VkQueue queue, VulkanBuffer* src_buffer, uint32_t dst_offset);
		static void CopyBufferToImage(VkCommandBuffer cmd, VkImageLayout dst_image_layout, VulkanBuffer* src_buffer, VkImage* dst_image, VkExtent3D image_extent);

		VkBuffer buffer = nullptr;
		VmaAllocation allocation = nullptr;
		uint64_t size;
	private:
		Vulkan_type* vulkan_type;
	};


	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer(Vulkan_type* vulkan_type,const std::vector<Vertex>& vertices);
		~VulkanVertexBuffer() override;
		void Bind() const override;
		void UnBind() const override {};
		void AddData(const std::vector<Vertex>& vertices);
		uint64_t GetCount() { return count; }

	private:
		Vulkan_type* vulkan_type;
		std::shared_ptr<VulkanBuffer> buffer;

		uint64_t count = 0;
		uint64_t buffer_size;
	};

	class VulkanIndexBuffer : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(Vulkan_type* vulkan_type , const std::vector<uint32_t>& data);
		~VulkanIndexBuffer() override;
		void Bind() const override;
		void UnBind() const override {}

		void AddData(const std::vector<uint32_t>& data);
		uint64_t GetCount() { return count; }
	private:
		Vulkan_type* vulkan_type;

		uint64_t buffer_size;
		std::shared_ptr<VulkanBuffer> buffer;
		uint64_t count = 0;
	};

	class VulkanUniformBuffer : public UniformBuffer
	{
	public:
		VulkanUniformBuffer(Vulkan_type* vulkan_type, uint32_t set_num);
		~VulkanUniformBuffer() override;

		void Bind() const override;
		void UnBind() const override;
		void ShutDown() override;

		// update cpu data block of member variable with matching name
		int UpdateData(const char* member_var_name, void* data) override;

		// return -1 ,if binding already exists or low binding_num doesn't exists
		int AddBinding(uint32_t binding_num, uint32_t buffer_size);

		int AddData(void* data, uint32_t offset, uint32_t buffer_size) override;
		
		// Map memory CPU to GPU
		void UploadToGPU(uint32_t offset = 0, uint32_t upload_size = 0);

		void SetupDescriptorSet(VkDescriptorSetLayout layout);
		
		VkDescriptorSet descriptor_set[MAX_FRAMES_IN_FLIGHT];
		uint64_t GetBufferSize() const;
	private:		
		Vulkan_type* vulkan_type;

		uint32_t set_num;
		
		struct binding_info
		{
			uint32_t binding;
			uint32_t offset;
			uint32_t size;
		};
		
		std::vector<binding_info> bindings;
		std::shared_ptr<VulkanBuffer> buffer[MAX_FRAMES_IN_FLIGHT];
	};

}

#endif //!VULKAN_BUFFER_H
