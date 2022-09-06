#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include <map>
#include <memory>

#include "vulkan_type.inl"

#include "../mesh.h"

namespace Renderer
{
	enum class DataType
	{
		NONE = 0, BOOL, INT, INT2, INT3, INT4, FLOAT, FLOAT1, FLOAT2, FLOAT3, FLOAT4, MAT3, MAT4,
	};


	struct UniformBufferMember
	{
		std::string name;
		DataType type;
		uint32_t size;
		uint32_t offset;
	};


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


	class VulkanVertexBuffer 
	{
	public:
		VulkanVertexBuffer(Vulkan_type* vulkan_type,const std::vector<Vertex>& vertices);
		~VulkanVertexBuffer();
		void Bind() const;
		void UnBind() const{};
		void AddData(const std::vector<Vertex>& vertices);
		uint64_t GetCount() { return count; }

	private:
		Vulkan_type* vulkan_type;
		std::shared_ptr<VulkanBuffer> buffer;

		uint64_t count = 0;
		uint64_t buffer_size;
	};

	class VulkanIndexBuffer 
	{
	public:
		VulkanIndexBuffer(Vulkan_type* vulkan_type , const std::vector<uint32_t>& data);
		~VulkanIndexBuffer();
		void Bind() const;
		void UnBind() const {}

		void AddData(const std::vector<uint32_t>& data);
		uint64_t GetCount() { return count; }
	private:
		Vulkan_type* vulkan_type;

		uint64_t buffer_size;
		std::shared_ptr<VulkanBuffer> buffer;
		uint64_t count = 0;
	};

	class VulkanUniformBuffer 
	{
	public:
		VulkanUniformBuffer(Vulkan_type* vulkan_type, uint32_t set_num);
		~VulkanUniformBuffer();

		void Bind() const;
		void UnBind() const;
		void ShutDown();
		void AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset);

		// update cpu data block of member variable with matching name
		int UpdateData(const char* member_var_name, void* data) ;

		// return -1 ,if binding already exists or low binding_num doesn't exists
		int AddBinding(uint32_t binding_num, uint32_t buffer_size);

		int AddData(void* data, uint32_t offset, uint32_t buffer_size);
		
		// Map memory CPU to GPU
		void UploadToGPU(uint32_t offset = 0, uint32_t upload_size = 0);

		void SetupDescriptorSet(VkDescriptorSetLayout layout);
		
		VkDescriptorSet descriptor_set[MAX_FRAMES_IN_FLIGHT];
		uint64_t GetBufferSize() const;


		std::map<std::string, UniformBufferMember> member_vars;

		void* data;
		uint64_t buffer_size;

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
