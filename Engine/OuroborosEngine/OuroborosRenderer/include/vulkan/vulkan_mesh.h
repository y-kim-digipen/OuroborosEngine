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
		VulkanMesh(VulkanType* vulkan_type);
		~VulkanMesh() override;

		bool CopyAssetData(const Asset::Mesh& mesh) override;
		void Draw(const glm::mat4& model, const glm::mat3& normal_matrix) override;

		std::unique_ptr<VulkanVertexBuffer> p_vertex_buffer;
		std::unique_ptr<VulkanIndexBuffer> p_index_buffer;
	private:
		VulkanType* vulkan_type;
	};
}

#endif // !VULKAN_MESH_H
