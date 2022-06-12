#pragma once
#include <vector>
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

namespace Asset
{
    struct Mesh
    {
        struct Vertex
        {
            glm::vec3 position;
            glm::vec3 normal;
            glm::vec2 uv;
    
            bool operator==(const Mesh::Vertex& other) const
            {
                return position == other.position && normal == other.normal && uv == other.uv;
            }
        };
    
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;
    };
    
    struct Image
    {
        unsigned char* image;
        int width, height;
    };
}

namespace std {
    template<> struct hash<Asset::Mesh::Vertex> {
        size_t operator()(Asset::Mesh::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}