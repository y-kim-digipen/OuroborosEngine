#ifndef MESH_H
#define MESH_H

#include <vector>
#include <vec3.hpp>
#include <vec2.hpp>
#include <glm\matrix.hpp>
//#include "assets.h"
#include <common/assets.h>

namespace Renderer {
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
		
		virtual bool CopyAssetData(const Asset::Mesh& mesh_name);
		virtual void Draw(const glm::mat4& model, const glm::mat4& normal_matrix) {}
	protected:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;
	};
};
#endif // !MESH_H

