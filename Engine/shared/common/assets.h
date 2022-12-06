#pragma once
#include <vector>
#include <map>
#include <glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include "shared.h"

namespace Asset
{
    struct Asset
    {
    };

	struct MeshPayload
	{
		uint64_t vertex_offset;
		uint64_t vertex_using_count;
		uint64_t index_offset;
		uint64_t index_using_count;
	};

    struct Mesh : Asset
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
		std::map<std::string, MeshPayload> payload_datas;

        inline static std::list<std::string> supported_formats { ".obj", ".fbx"};
    };
    
    struct Image : Asset
    {
        unsigned char* image;
        int width, height;
        int channel;
        std::string filename;
        inline static std::list<std::string> supported_formats{ ".jpg", ".jpeg", ".png" };
    };

    struct Shader : Asset
    {
        std::string name;
        inline static std::list<std::string> supported_formats{ ".vert", ".frag" };
    };

    struct Script : Asset
    {
        std::string path;
        inline static std::list<std::string> supported_formats{ ".lua" , ".clua", ".slua"};
    };

    struct CameraData : Asset
    {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 inv_view;
        alignas(16) glm::vec3 position;
    };

    struct Material : Asset
    {
        Material() = default;
        Material(const Material& other)
        {
            *this = other;
        }
        Material& operator=(const Material& other)
        {
            name = other.name;
            pbr_mat = other.pbr_mat;
            return *this;
        }
        std::string name;
        Shared::PBR_Structure pbr_mat{Shared::MaterialMode::NONE_TEXTURE};
        inline static std::list<std::string> supported_formats;
    };

    struct MaterialData : Asset
    {
        alignas(16)
    	glm::vec3 albedo;
        float metallic;
        float roughness;
        float ao;
        int has_albedo_texture=0;
        int has_normal_texture=0;
        int has_metalroughness_texture=0;
        int has_ao_texture=0;
        int has_metalic_texture = 0;
        int has_roughness_texture = 0;
        int has_emissive_texture = 0;
        int is_smoothness = 0;
    };

    struct LightData : Asset
    {
        alignas(16)   glm::vec3 pos;
		float cutoff;
        alignas(16)   glm::vec3 diffuse;
		float out_cutoff;
        alignas(16) 	glm::vec3 dir={0,-1,0};
        alignas(16) glm::mat4 view_matrix = glm::mat4(1.f);
        float falloff;
        int type; // 0 = point, 1 = spot, 2 = directional

    };

    struct LightShadowData : Asset
    {
        glm::mat4 mvp[20];
    };
}

namespace std {
    template<> struct hash<Asset::Mesh::Vertex> {
        size_t operator()(Asset::Mesh::Vertex const& vertex) const {
            return ((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.normal) << 1)) >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1);
        }
    };
}