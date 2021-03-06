#ifndef BUFFER_H
#define BUFFER_H

#include <map>
#include <string>

namespace Renderer
{
	enum class DataType
	{
		NONE = 0, BOOL, INT, INT2, INT3, INT4, FLOAT, FLOAT1, FLOAT2, FLOAT3, FLOAT4, MAT3, MAT4,
	};


	class VertexBuffer
	{
	public:
		VertexBuffer() = default;
		virtual ~VertexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const {}
	};

	class IndexBuffer
	{
	public:
		IndexBuffer() = default;
		virtual ~IndexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const {}
	};

	struct UniformBufferMember {
		std::string name;
		DataType type;
		uint32_t size;
		uint32_t offset;
	};

	class UniformBuffer
	{
	public:
		UniformBuffer() = default;
		virtual ~UniformBuffer() = default;
		virtual void Bind() const = 0;
		virtual void UnBind() const {}
		virtual void ShutDown();
		void AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset);
		virtual int AddData(void* data, uint32_t offset, uint32_t buffer_size) = 0;
		virtual int UpdateData(const char* member_var_name, void* data);

		//TODO: seperate colors with other val
		std::map<std::string, UniformBufferMember> member_vars;

		void* data;
		uint64_t buffer_size;
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();


	};



}






#endif
