#ifndef VULKAN_MESH_H
#define VULKAN_MESH_H

#include "../mesh.h"
#include "vulkan_buffer.h"
#include <memory>

struct VulkanType;

namespace Renderer {

	//class VulkanVertexBuffer;
	//class VulkanIndexBuffer;

	class VulkanMesh : public Mesh {
	public:
		VulkanMesh(VulkanType* vulkan_type, Model* parent_model, uint64_t vertex_offset, uint64_t index_offset, uint64_t index_count);
		~VulkanMesh() override;

		void Draw(const glm::mat4& model, const glm::mat4& normal_matrix) override;

	private:
		VulkanType* vulkan_type;
		Model* parent_model;
		uint64_t vertex_offset;
		uint64_t index_offset;
		uint64_t index_count;
	};
}

#endif // !VULKAN_MESH_H
