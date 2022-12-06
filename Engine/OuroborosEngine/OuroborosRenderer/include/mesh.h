#ifndef MESH_H
#define MESH_H

#include <memory>

#include <vector>
#include <vec3.hpp>
#include <vec2.hpp>
#include <glm\matrix.hpp>
//#include "assets.h"
#include <common/assets.h>

struct VulkanType;

namespace Renderer {
	class VulkanMesh;
	class VulkanVertexBuffer;
	class VulkanIndexBuffer;

	struct Vertex {
		glm::vec3 pos;
		glm::vec3 normal;
		glm::vec2 uv;

		bool operator==(const Vertex& other) const {
			return pos == other.pos && normal == other.normal && uv == other.uv;
		}

		static size_t offset(const char* member) {

			size_t offset = 0;

			if (strcmp(member, "normal") == 0) {
				offset = offsetof(Vertex, normal);
			}
			else if (strcmp(member, "pos") == 0) {
				offset = offsetof(Vertex, pos);
			}
			else if (strcmp(member, "uv") == 0) {
				offset = offsetof(Vertex, uv);
			}

			return offset;
		}
	};

	class Mesh {
	public:
		Mesh() = default;
		virtual ~Mesh() =default;
		
		virtual void Draw(const glm::mat4& model, const glm::mat4& normal_matrix) {}
	protected:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};

	struct Model {
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
		std::unique_ptr<VulkanVertexBuffer> p_vertex_buffer;
		std::unique_ptr<VulkanIndexBuffer> p_index_buffer;
		
		std::vector<std::unique_ptr<VulkanMesh>> mesh_list;
	};

	bool InitModel(VulkanType* vulkan_type, Model* out_model, const Asset::Mesh& mesh);
	bool BindModel(Model* model);
	void CleanupModel(Model* model);
};
#endif // !MESH_H

