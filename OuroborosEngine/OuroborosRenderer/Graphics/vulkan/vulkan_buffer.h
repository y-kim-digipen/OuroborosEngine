#ifndef VULKAN_BUFFER_H
#define VULKAN_BUFFER_H
#include "../buffer.h"

namespace Renderer
{

	class VulkanVertexBuffer : public VertexBuffer
	{
	public:
		VulkanVertexBuffer();
		void Bind() const override;
		void UnBind() const override;


	private:


	};
}



#endif //!VULKAN_BUFFER_H
