#ifndef BUFFER_H
#define BUFFER_H

namespace Renderer
{
	enum class DataType
	{
		NONE = 0, BOOL, INT, INT2, INT3, INT4, FLOAT, FLOAT1, FLOAT2, FLOAT3, FLOAT4, MAT3, MAT4,
	};



	class VertexBuffer
	{
	public:
		VertexBuffer(float* vertices, unsigned size);
		~VertexBuffer() = default;
		virtual void Bind() const;
		virtual void UnBind() const {}
	};

	class IndexBuffer
	{
		IndexBuffer();
		~IndexBuffer() = default;
		virtual void Bind() const;
		virtual void UnBind() const {}
	};

	class VertexArray
	{
		VertexArray();
		~VertexArray();


	};



}






#endif
