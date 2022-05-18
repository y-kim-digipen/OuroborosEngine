#ifndef MESH_H
#define MESH_H

#include <vector>
#include <vec3.hpp>
#include <vec2.hpp>

struct Vertex {
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec2 uv;

	bool operator==(const Vertex& other) const {
		return position == other.position && normal == other.normal && uv == other.uv;
	}
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<uint32_t> indices;

	bool load_asset(const char* file_name);
};

#endif // !MESH_H

