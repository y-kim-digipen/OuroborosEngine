#ifndef MESH_H
#define MESH_H

#include <vector>
#include <vec3.hpp>
#include <vec2.hpp>

namespace Renderer {
	struct Vertex {
		glm::vec3 position;
		glm::vec3 normal;
		glm::vec2 uv;

		bool operator==(const Vertex& other) const {
			return position == other.position && normal == other.normal && uv == other.uv;
		}
	};

	class Mesh {

	public:
		Mesh() = default;
		virtual ~Mesh() = 0 {};

		virtual bool LoadAsset(const char* file_name);
		virtual void Draw() = 0;

	protected:
		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

	};
};
#endif // !MESH_H

