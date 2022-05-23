#include "vulkan_buffer.h"
#include "vulkan_type.inl"

namespace Renderer
{
	VulkanBuffer::VulkanBuffer(VmaAllocator* allocator, uint64_t buffer_size, VkBufferUsageFlags buffer_usage,
		VmaMemoryUsage vma_usage) : allocator(allocator)
	{
		VkBufferCreateInfo buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_create_info.size = buffer_size;
		buffer_create_info.usage = buffer_usage;


		VmaAllocationCreateInfo vma_allocation_create_info{};
		vma_allocation_create_info.usage = vma_usage;

	
		auto result = vmaCreateBuffer(*allocator, &buffer_create_info, &vma_allocation_create_info, &buffer, &allocation, nullptr);
		if(result != VK_SUCCESS)
		{
			//Error handling
		}

	}

	VulkanBuffer::~VulkanBuffer()
	{
		vmaDestroyBuffer(*allocator, buffer, allocation);
	}

	void VulkanBuffer::UploadData(void* data, uint64_t buffer_size)
	{
		void* temp_data = nullptr;

		vmaMapMemory(*allocator, allocation, &temp_data);
		memcpy(temp_data, data, buffer_size);
		vmaUnmapMemory(*allocator, allocation);
	}

	void VulkanBuffer::CopyBuffer(VkQueue* queue, VulkanBuffer* src_buffer, VulkanBuffer* dst_buffer, VkDeviceSize size)
	{
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.level			     = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool        = vulkan_type.command_pool;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(vulkan_type.device.handle, &allocate_info, &command_buffer);


		VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegion
		{
			.srcOffset = 0,
			.dstOffset = 0,
			.size	   = size
		};

		vkCmdCopyBuffer(command_buffer, src_buffer->buffer, dst_buffer->buffer, 1, &copyRegion);
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &command_buffer;

		vkQueueSubmit(*queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(*queue);

		vkFreeCommandBuffers(vulkan_type.device.handle, vulkan_type.command_pool, 1, &command_buffer);

	}

	void VulkanBuffer::CopyBufferToImage(VkCommandBuffer cmd, VkImageLayout dst_image_layout, VulkanBuffer* src_buffer,
		VkImage* dst_image, VkExtent3D image_extent)
	{
		VkBufferImageCopy copyRegion = {};
		copyRegion.bufferOffset		 = 0;
		copyRegion.bufferRowLength   = 0;
		copyRegion.bufferImageHeight = 0;

		copyRegion.imageSubresource.aspectMask		= VK_IMAGE_ASPECT_COLOR_BIT;
		copyRegion.imageSubresource.mipLevel	    = 0;
		copyRegion.imageSubresource.baseArrayLayer	= 0;
		copyRegion.imageSubresource.layerCount		= 1;
		copyRegion.imageExtent						= image_extent;

		vkCmdCopyBufferToImage(cmd, src_buffer->buffer, *dst_image, dst_image_layout, 1, &copyRegion);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		buffer.reset();
		buffer = nullptr;
	}

	void VulkanVertexBuffer::Bind() const
	{
		if(buffer != nullptr)
		{
			VkDeviceSize offset = 0;
			auto& frame = vulkan_type.frame_data[vulkan_type.current_frame % MAX_FRAMES_IN_FLIGHT];
			vkCmdBindVertexBuffers(frame.command_buffer, 0, 1, &buffer->buffer, &offset);
		}
	}

	void VulkanVertexBuffer::AddData(const std::vector<Vertex>& vertices)
	{
		uint64_t new_buffer_size{ vertices.size() * sizeof(Vertex) };

		if(buffer_size != new_buffer_size)
		{
			if(buffer != nullptr)
			{
				buffer.reset();
			}
			buffer_size = new_buffer_size;
			count = static_cast<uint64_t>(vertices.size());

			buffer = std::make_shared<VulkanBuffer>(&vulkan_type.allocator, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(&vulkan_type.allocator, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		VulkanBuffer::CopyBuffer(&vulkan_type.device.graphics_queue, staging_buffer.get(), buffer.get(), buffer_size);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		buffer.reset();
		buffer = nullptr;
	}

	void VulkanIndexBuffer::Bind() const
	{
		if(buffer != nullptr)
		{
			VkDeviceSize offset = 0;
			auto& frame = vulkan_type.frame_data[vulkan_type.current_frame % MAX_FRAMES_IN_FLIGHT];
			vkCmdBindIndexBuffer(frame.command_buffer, buffer->buffer, offset, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanIndexBuffer::AddData(const std::vector<uint32_t>& data)
	{
		uint64_t new_buffer_size{ data.size() * sizeof(uint32_t) };

		if(buffer_size != new_buffer_size)
		{
			if(buffer != nullptr)
			{
				buffer.reset();
			}

			buffer_size = new_buffer_size;
			count = static_cast<uint64_t>(data.size());
			buffer = std::make_shared<VulkanBuffer>(&vulkan_type.allocator, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(&vulkan_type.allocator, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		VulkanBuffer::CopyBuffer(&vulkan_type.device.graphics_queue, staging_buffer.get(), buffer.get(), buffer_size);
	}

}
