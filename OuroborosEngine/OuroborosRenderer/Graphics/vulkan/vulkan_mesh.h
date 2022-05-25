#ifndef VULKAN_MESH_H
#define VULKAN_MESH_H

#include "../mesh.h"

#include <memory>

namespace Renderer {

	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	struct VulkanMesh : Mesh {

		std::unique_ptr<VulkanVertexBuffer> p_vertex_buffer;
		std::unique_ptr<VulkanIndexBuffer> p_index_buffer;



	};
}

#endif // !VULKAN_MESH_H
