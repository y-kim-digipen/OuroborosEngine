#include "vulkan_buffer.h"

namespace Renderer
{
	VulkanBuffer::VulkanBuffer(Vulkan_type* vulkan_type, uint64_t buffer_size, VkBufferUsageFlags buffer_usage,
		VmaMemoryUsage vma_usage) : vulkan_type(vulkan_type) ,size(buffer_size) {
		VkBufferCreateInfo buffer_create_info{ VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO };
		buffer_create_info.size = buffer_size;
		buffer_create_info.usage = buffer_usage;


		VmaAllocationCreateInfo vma_allocation_create_info{};
		vma_allocation_create_info.usage = vma_usage;

		
		VkResult result = vmaCreateBuffer(vulkan_type->allocator, &buffer_create_info, &vma_allocation_create_info, &buffer, &allocation, nullptr);
		if(result != VK_SUCCESS)
		{
			//Error handling
		}

	}

	VulkanBuffer::~VulkanBuffer()
	{
		vmaDestroyBuffer(vulkan_type->allocator, buffer, allocation);
	}

	void VulkanBuffer::UploadData(void* data, uint64_t buffer_size)
	{
		void* temp_data = nullptr;

		vmaMapMemory(vulkan_type->allocator, allocation, & temp_data);
		memcpy(temp_data, data, buffer_size);
		vmaUnmapMemory(vulkan_type->allocator, allocation);
	}

	void VulkanBuffer::CopyBuffer(VkQueue queue, VulkanBuffer* src_buffer)
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
			.dstOffset = 0,
			.size	   = size
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

	VulkanVertexBuffer::VulkanVertexBuffer(Vulkan_type* vulkan_type, const std::vector<Vertex>& vertices) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ vertices.size() * sizeof(Vertex) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(vertices.size());

		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get());
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
			auto& frame = vulkan_type->frame_data[vulkan_type->current_frame];
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

			buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get());
	}

	VulkanIndexBuffer::VulkanIndexBuffer(Vulkan_type* vulkan_type ,const std::vector<uint32_t>& data) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ data.size() * sizeof(uint32_t) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(data.size());
		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get());
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
			auto& frame = vulkan_type->frame_data[vulkan_type->current_frame];
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
			buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);
		}

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get());
	}


	VulkanUniformBuffer::VulkanUniformBuffer(Vulkan_type* vulkan_type, uint32_t buffer_size) : vulkan_type(vulkan_type)
	{
		data = malloc(buffer_size);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			buffer[i] = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_MEMORY_USAGE_CPU_TO_GPU);			
		}
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		free(data);
		data = nullptr;
	}

	void VulkanUniformBuffer::Bind() const
	{
		buffer[vulkan_type->current_frame]->UploadData(data, buffer_size);

	
	}

	void VulkanUniformBuffer::UnBind() const
	{

		UniformBuffer::UnBind();

		
	}

	int VulkanUniformBuffer::UpdateData(const char* member_var_name, void* data)
	{
		if (member_vars.find(member_var_name) != member_vars.end()) 
		{
			memcpy(&this->data + member_vars[member_var_name].offset, data, member_vars[member_var_name].size);
			return 0;
		}

		return -1;
	}

	void VulkanUniformBuffer::SetupDescriptorSet(uint32_t binding, uint32_t descriptor_count, VkDescriptorSetLayout layout)
	{
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
			alloc_info.descriptorPool = vulkan_type->descriptor_pool;
			alloc_info.descriptorSetCount = 1;
			alloc_info.pSetLayouts = &layout;
	
			VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &descriptor_set[i]));

			VkDescriptorBufferInfo buffer_info{};
			buffer_info.buffer = buffer[i]->buffer;
			buffer_info.offset = 0;
			buffer_info.range = buffer[i]->size;

			VkWriteDescriptorSet set_write{ VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET };
			set_write.dstSet = descriptor_set[i];
			set_write.dstBinding = binding;
			set_write.descriptorCount = 1;
			set_write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			set_write.pBufferInfo = &buffer_info;

			vkUpdateDescriptorSets(vulkan_type->device.handle, 1, &set_write, 0, nullptr);
		}
	}
}
