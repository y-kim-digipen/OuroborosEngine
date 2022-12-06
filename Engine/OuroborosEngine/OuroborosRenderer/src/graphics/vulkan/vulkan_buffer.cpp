#include "vulkan_buffer.h"

#include <iostream>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

namespace Renderer
{
	VulkanBuffer::VulkanBuffer(VulkanType* vulkan_type, uint64_t buffer_size, VkBufferUsageFlags buffer_usage,
		VmaMemoryUsage vma_usage) : vulkan_type(vulkan_type) ,size(buffer_size) {
		VkBufferCreateInfo buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_create_info.size = buffer_size;
		buffer_create_info.usage = buffer_usage;


		VmaAllocationCreateInfo vma_allocation_create_info{};
		vma_allocation_create_info.usage = vma_usage;

		
		VkResult result = vmaCreateBuffer(vulkan_type->allocator, &buffer_create_info, &vma_allocation_create_info, &buffer, &allocation, nullptr);

//Austyn Park (debug memory leak)
// Must disable VMA_DEBUG_LOG in vk_mem_alloc.h
#if defined(_DEBUG) 
		switch (buffer_usage)
		{
		case VK_BUFFER_USAGE_TRANSFER_SRC_BIT:
			vmaSetAllocationName(vulkan_type->allocator, allocation, "transfer_src");
			break;
		case VK_BUFFER_USAGE_TRANSFER_DST_BIT:
			vmaSetAllocationName(vulkan_type->allocator, allocation, "transfer_dst");
			break;
		case VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT:
			vmaSetAllocationName(vulkan_type->allocator, allocation, "uniform");
			break;
		case VK_BUFFER_USAGE_INDEX_BUFFER_BIT:
			vmaSetAllocationName(vulkan_type->allocator, allocation, "index");
			break;
		case VK_BUFFER_USAGE_VERTEX_BUFFER_BIT:
			vmaSetAllocationName(vulkan_type->allocator, allocation, "vertex");
			break;
		default:
			break;
		}
#endif

		if(result != VK_SUCCESS)
		{
			//Error handling
		}

	}

	VulkanBuffer::~VulkanBuffer()
	{
		if (buffer != VK_NULL_HANDLE) {
			vmaDestroyBuffer(vulkan_type->allocator, buffer, allocation);
		}
	}


	void VulkanBuffer::UploadData(void* data, uint64_t buffer_size, uint32_t offset)
	{
		char* temp_data = nullptr;

		vmaMapMemory(vulkan_type->allocator, allocation, (void**)&temp_data);
		temp_data += offset;
		memcpy(temp_data, data, buffer_size);
		vmaUnmapMemory(vulkan_type->allocator, allocation);
	}

	void VulkanBuffer::CopyBuffer(VkQueue queue, VulkanBuffer* src_buffer, uint32_t dst_offset)
	{
		VkCommandBufferAllocateInfo allocate_info{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO };
		allocate_info.level			     = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocate_info.commandPool        = vulkan_type->command_pool;
		allocate_info.commandBufferCount = 1;

		VkCommandBuffer command_buffer;
		vkAllocateCommandBuffers(vulkan_type->device.handle, &allocate_info, &command_buffer);


		VkCommandBufferBeginInfo commandBufferBeginInfo{ VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO };
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(command_buffer, &commandBufferBeginInfo);

		VkBufferCopy copyRegion
		{
			.srcOffset = 0,
			.dstOffset = dst_offset,
			.size = src_buffer->size
		};

		vkCmdCopyBuffer(command_buffer, src_buffer->buffer, buffer, 1, &copyRegion);
		vkEndCommandBuffer(command_buffer);

		VkSubmitInfo submitInfo{ VK_STRUCTURE_TYPE_SUBMIT_INFO };
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers    = &command_buffer;

		vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(queue);

		vkFreeCommandBuffers(vulkan_type->device.handle, vulkan_type->command_pool, 1, &command_buffer);
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

	void VulkanBuffer::Cleanup()
	{
		if (buffer != VK_NULL_HANDLE) {
			vmaDestroyBuffer(vulkan_type->allocator, buffer, allocation);
			buffer = VK_NULL_HANDLE;
		}
	}

	VkDescriptorBufferInfo VulkanBuffer::GetBufferInfo() const
	{
		return {
			buffer,
			0,
			size
		};
	}

	VulkanVertexBuffer::VulkanVertexBuffer(VulkanType* vulkan_type, const std::vector<Vertex>& vertices) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ vertices.size() * sizeof(Vertex) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(vertices.size());

		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);

		staging_buffer->Cleanup();
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		buffer->Cleanup();
		buffer.reset();
		buffer = nullptr;
	}

	void VulkanVertexBuffer::Bind(uint64_t vertex_offset) const
	{
		if(buffer != nullptr)
		{
			auto& frame = vulkan_type->frame_data[vulkan_type->current_frame];
			vkCmdBindVertexBuffers(frame.command_buffer, 0, 1, &buffer->buffer, &vertex_offset);
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

			buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
		staging_buffer->Cleanup();
	}

	void VulkanVertexBuffer::Cleanup()
	{
		if (buffer != nullptr) {
			buffer->Cleanup();
			buffer.reset();
			buffer = nullptr;
		}
	}

	VulkanIndexBuffer::VulkanIndexBuffer(VulkanType* vulkan_type ,const std::vector<uint32_t>& data) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ data.size() * sizeof(uint32_t) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(data.size());
		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
		staging_buffer->Cleanup();
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		buffer.reset();
		buffer = nullptr;
	}

	void VulkanIndexBuffer::Bind(uint64_t offset) const
	{
		if(buffer != nullptr)
		{
			auto& frame = vulkan_type->frame_data[vulkan_type->current_frame];
			vkCmdBindIndexBuffer(frame.command_buffer, buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
		}
	}

	void VulkanIndexBuffer::Cleanup()
	{
		if (buffer != nullptr) {
			buffer->Cleanup();
			buffer.reset();
			buffer = nullptr;
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
			buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
		staging_buffer->Cleanup();
	}


	VulkanUniformBuffer::VulkanUniformBuffer(VulkanType* vulkan_type, uint32_t binding_num, uint32_t size)	{
		Init(vulkan_type, binding_num, size);
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		Cleanup();
	}

	void VulkanUniformBuffer::Init(VulkanType* vulkan_type, uint32_t binding_num, uint32_t size)
	{
		this->vulkan_type = vulkan_type;
		block_info.binding = binding_num;
		block_info.size = size;
		//block_info.size = pad_uniform_buffer_size(vulkan_type->device.properties.limits.minUniformBufferOffsetAlignment, size);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			buffer[i] = std::make_shared<VulkanBuffer>(vulkan_type, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);
		}
		
		if (size != 0) {
			data = malloc(size);
			memset(data, 0, size);
		}
	}

	void VulkanUniformBuffer::Cleanup()
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			if (buffer[i] != nullptr)
				buffer[i]->Cleanup();
				buffer[i].reset();
		}
	}

	int VulkanUniformBuffer::UpdateData(const char* member_var_name, void* data_)
	{
		auto itr = member_vars.find(member_var_name);

		if (itr != member_vars.end()) 
		{
			AddData(data_, member_vars[member_var_name].offset, member_vars[member_var_name].size);
			return 0;
		}

		return -1;
	}

	int VulkanUniformBuffer::AddData(void* data_, uint32_t offset, uint32_t buffer_size)
	{
		if (block_info.size < buffer_size)
		{
			std::cout << "block size is bigger than the total block size!" << std::endl;
			return  -1;
		}

		char* mem = (char*)data + offset;
		memcpy_s(mem, block_info.size, data_, buffer_size);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			buffer[i]->UploadData(data_, buffer_size, offset);
		}

		return 0;
	}

	uint64_t VulkanUniformBuffer::GetBufferSize() const
	{
		return block_info.size;
	}

	VkDescriptorBufferInfo VulkanUniformBuffer::GetBufferInfo(uint32_t frame_num) const
	{
		return buffer[frame_num]->GetBufferInfo();
	}

	void VulkanUniformBuffer::AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset)
	{
		//size = block_info.size = pad_uniform_buffer_size(vulkan_type->device.properties.limits.minUniformBufferOffsetAlignment, size);

		if (size + offset > block_info.size) {
			std::cout << name << " member data is out of a uniform buffer size range" << std::endl;
			return;
		}

		if (member_vars.find(name) == member_vars.end()) {
			member_vars[name].name = name;
			member_vars[name].type = data_type;
			member_vars[name].size = size;
			member_vars[name].offset = offset;
		}
	}
	size_t pad_uniform_buffer_size(size_t min_ubo_alignment, size_t originalSize)
	{
		size_t alignedSize = originalSize;

		if (min_ubo_alignment > 0) {
			alignedSize = (alignedSize + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
		}

		return alignedSize;
	}
}
