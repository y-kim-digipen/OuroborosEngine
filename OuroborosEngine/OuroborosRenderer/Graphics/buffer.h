#ifndef BUFFER_H
#define BUFFER_H

#include <unordered_map>
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
		void AddMember(const std::string& name, DataType data_type, uint32_t size, uint32_t offset);

		std::unordered_map<std::string, UniformBufferMember> member_vars;
		// members
		// member name, type, size, offset
	};

	class VertexArray
	{
	public:
		VertexArray();
		~VertexArray();


	};



}






#endif
