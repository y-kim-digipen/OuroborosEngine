#ifndef VULKAN_MESH_H
#define VULKAN_MESH_H

#include "../mesh.h"
#include "vulkan_buffer.h"
#include <memory>

namespace Renderer {

	//class VulkanVertexBuffer;
	//class VulkanIndexBuffer;

	class VulkanMesh : public Mesh {
	public:
		VulkanMesh() = default;
		~VulkanMesh() override;

		bool LoadAsset(const char* file_name) override;
		void Draw() override;

		std::unique_ptr<VulkanVertexBuffer> p_vertex_buffer;
		std::unique_ptr<VulkanIndexBuffer> p_index_buffer;
	};
}

#endif // !VULKAN_MESH_H
