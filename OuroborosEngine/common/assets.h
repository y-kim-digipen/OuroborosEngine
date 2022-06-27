#pragma once
#include <vector>
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <imgui-docking/imgui.h>

#include "../../OuroborosRenderer/Graphics/buffer.h"

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

        inline static std::list<std::string> supported_formats { ".obj" };
    };
    
    struct Image
    {
        unsigned char* image;
        int width, height;
        
        inline static std::list<std::string> supported_formats{ ".jpg", ".jpeg", ".png" };
    };

    struct Shader
    {
        std::string name;

        inline static std::list<std::string> supported_formats{ ".vert", ".frag" };
    };

    struct CameraData
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::vec3 position;
    };

    struct MaterialData
    {
        alignas(16)  glm::vec3 ambient;
        alignas(16)  glm::vec3 diffuse;
        alignas(16)  glm::vec3 specular;
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

#include "asset_imgui_impl.inl"
