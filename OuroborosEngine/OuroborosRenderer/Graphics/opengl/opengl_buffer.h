#ifndef OPENGL_BUFFER_H
#define OPENGL_BUFFER_H

#include "../buffer.h"

namespace Renderer
{
	






	class OpenglVertexBuffer : public VertexBuffer
	{
	public:
		OpenglVertexBuffer(unsigned int size);
		OpenglVertexBuffer(float* vertices, unsigned size);
		~OpenglVertexBuffer() override;
		void BufferData(const void* data, unsigned size);
		void Bind() const override;
		void UnBind() const override;
	private:
		unsigned int render_id = std::numeric_limits<unsigned int>::max();
	};




}










#endif !OPENGL_BUFFER_H