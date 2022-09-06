#include "vulkan_buffer.h"

#include <iostream>
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

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

	VulkanVertexBuffer::VulkanVertexBuffer(Vulkan_type* vulkan_type, const std::vector<Vertex>& vertices) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ vertices.size() * sizeof(Vertex) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(vertices.size());

		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)vertices.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		buffer->Cleanup();
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

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
	}

	void VulkanVertexBuffer::Cleanup()
	{
		if (buffer != nullptr) {
			buffer->Cleanup();
			buffer.reset();
			buffer = nullptr;
		}
	}

	VulkanIndexBuffer::VulkanIndexBuffer(Vulkan_type* vulkan_type ,const std::vector<uint32_t>& data) : vulkan_type(vulkan_type)
	{
		uint64_t new_buffer_size{ data.size() * sizeof(uint32_t) };
		buffer_size = new_buffer_size;
		count = static_cast<uint64_t>(data.size());
		buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData((int*)data.data(), buffer_size);

		buffer->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), 0);
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
	}


	VulkanUniformBuffer::VulkanUniformBuffer(Vulkan_type* vulkan_type, uint32_t set_num) :  vulkan_type(vulkan_type), set_num(set_num)
	{
		data = nullptr;
	}

	VulkanUniformBuffer::~VulkanUniformBuffer()
	{
		free(data);
		data = nullptr;
	}

	void VulkanUniformBuffer::Bind() const
	{
		if (vulkan_type->current_pipeline_layout != VK_NULL_HANDLE)
			vkCmdBindDescriptorSets(vulkan_type->frame_data[vulkan_type->current_frame].command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_type->current_pipeline_layout, set_num, 1, &descriptor_set[vulkan_type->current_frame], 0, nullptr);
	}

	void VulkanUniformBuffer::UnBind() const
	{
		
	}

	void VulkanUniformBuffer::ShutDown()
	{
		if (data != nullptr)
			free(data);
		data = nullptr;

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			buffer[i].reset();
	}

	int VulkanUniformBuffer::UpdateData(const char* member_var_name, void* data_)
	{
		if (member_vars.find(member_var_name) != member_vars.end()) 
		{
			char* ptr = reinterpret_cast<char*>(this->data) + member_vars[member_var_name].offset;
			memcpy(ptr , data_, member_vars[member_var_name].size);
			UploadToGPU(member_vars[member_var_name].offset,member_vars[member_var_name].size);
			return 0;
		}

		return -1;
	}

	int VulkanUniformBuffer::AddBinding(uint32_t binding_num, uint32_t buffer_size)
	{
		uint32_t current_binding_count = bindings.size();

		if (current_binding_count > binding_num) {
			std::cout << "Failed to add binding " << binding_num << std::endl;
			return -1;
		}

		uint32_t offset = 0;

		if (current_binding_count > 0) {
			offset = bindings.back().offset + bindings.back().size;

			// Calculate required alignment based on minimum device offset alignment
			size_t min_ubo_alignment = vulkan_type->device.properties.limits.minUniformBufferOffsetAlignment;
			size_t aligned_size = offset;
			if (min_ubo_alignment > 0) {
				aligned_size = (aligned_size + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
			}
				
			offset = aligned_size;
		}

		bindings.emplace_back(
			binding_num,
			offset,
			buffer_size
		);
	}

	// offset refers ubo block offset
	int VulkanUniformBuffer::AddData(void* data, uint32_t offset, uint32_t buffer_size)
	{
		if (this->buffer_size < buffer_size)
		{
			std::cout << "block size is bigger than the total block size!" << std::endl;
			return  -1;
		}


		// Calculate required alignment based on minimum device offset alignment
		size_t min_ubo_alignment = vulkan_type->device.properties.limits.minUniformBufferOffsetAlignment;
		size_t aligned_size = offset;
		if (min_ubo_alignment > 0) {
			aligned_size = (aligned_size + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
		}

		offset = aligned_size;

		char* mem = (char*)this->data + offset;

		memcpy_s(mem, this->buffer_size, data, buffer_size);

		return 0;
	}

	void VulkanUniformBuffer::UploadToGPU(uint32_t offset, uint32_t upload_size)
	{
		if (upload_size == 0)
			upload_size = this->buffer_size;

		auto staging_buffer = std::make_shared<VulkanBuffer>(vulkan_type, upload_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
		staging_buffer->UploadData(data, upload_size, offset);

		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			buffer[i]->CopyBuffer(vulkan_type->device.graphics_queue, staging_buffer.get(), offset);
	}

	void VulkanUniformBuffer::SetupDescriptorSet(VkDescriptorSetLayout layout)
	{
		uint32_t binding_count = bindings.size();

		if (binding_count < 0)
		{
			std::cout << "Cannot set up descriptor set! binding doesn't exists" << std::endl;
			return;
		}
		
		if (buffer_size == 0 || data == nullptr) {
			buffer_size = bindings.back().offset + bindings.back().size;
			data = malloc(buffer_size);
			memset(data, 0, buffer_size);
		}
		VkDescriptorSetAllocateInfo alloc_info{ VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO };
		alloc_info.descriptorPool = vulkan_type->descriptor_pool;
		alloc_info.descriptorSetCount = 1;
		alloc_info.pSetLayouts = &layout;


		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {

			buffer[i] = std::make_shared<VulkanBuffer>(vulkan_type, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VMA_MEMORY_USAGE_GPU_ONLY);

			VK_CHECK(vkAllocateDescriptorSets(vulkan_type->device.handle, &alloc_info, &descriptor_set[i]));

			std::vector<VkWriteDescriptorSet> set_writes(binding_count);
			std::vector<VkDescriptorBufferInfo> buffer_info(binding_count);

			for (uint32_t i_binding = 0; i_binding < binding_count; ++i_binding) {

				buffer_info[i_binding] = {};
				buffer_info[i_binding].buffer = buffer[i]->buffer;
				buffer_info[i_binding].offset = bindings[i_binding].offset;
				buffer_info[i_binding].range = bindings[i_binding].size;

				set_writes[i_binding] = {VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET};
				set_writes[i_binding].dstSet = descriptor_set[i];
				set_writes[i_binding].dstBinding = bindings[i_binding].binding;
				set_writes[i_binding].descriptorCount = 1;
				set_writes[i_binding].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
				set_writes[i_binding].pBufferInfo = &buffer_info[i_binding];
			}

			vkUpdateDescriptorSets(vulkan_type->device.handle, binding_count, set_writes.data(), 0, nullptr);
		}
	}

	uint64_t VulkanUniformBuffer::GetBufferSize() const
	{
		return buffer_size;
	}


	//temporary


	void VulkanUniformBuffer::AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset)
	{
		if (member_vars.find(name) == member_vars.end()) {
			member_vars[name].name = name;
			member_vars[name].type = data_type;
			member_vars[name].size = size;
			member_vars[name].offset = offset;
		}
	}

	//int UniformBuffer::UpdateData(const char* member_var_name, void* data)
	//{
	//	if (member_vars.find(member_var_name) != member_vars.end())
	//	{
	//		const UniformBufferMember& ub_mem = member_vars[member_var_name];
	//		memcpy_s((char*)this->data + ub_mem.offset, ub_mem.size, data, ub_mem.size);

	//		return 0;
	//	}

	//	return -1;
	//}

}
