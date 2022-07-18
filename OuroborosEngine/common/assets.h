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
        int channel;
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
        alignas(16)  glm::vec3 albedo;
        float metallic;
        float roughness;
        float ao;
        int has_albedo_texture=0;
        int has_normal_texture=0;
        int has_metalroughness_texture=0;
        int has_ao_texture=0;
    };

    struct LightData
    {
        alignas(16)   glm::vec3 pos;
		float cutoff;
        alignas(16)   glm::vec3 diffuse;
		float out_cutoff;
        alignas(16) 	glm::vec3 dir;
        float falloff;
        int type; // 0 = point, 1 = spot, 2 = directional
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
