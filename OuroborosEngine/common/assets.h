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

    struct CameraData
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 position;
    };

    struct MaterialData
    {
        glm::vec3 ambient;
        glm::vec3 diffuse;
        glm::vec3 specular;
        float shininess;
    };

    struct LightData
    {
        glm::vec3 position;
        float cutoff;
        glm::vec3 diffuse;
        float out_cutoff;
        glm::vec3 ambient;
        float falloff;
        glm::vec3 specular;
        float padding4;
        glm::vec3 direction;
        int light_type;
    };
}

namespace std {
    template<> struct hash<Asset::Mesh::Vertex> {
        size_t operator()(Asset::Mesh::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}