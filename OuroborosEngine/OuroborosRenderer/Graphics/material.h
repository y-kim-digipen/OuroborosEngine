#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../common/assets.h"
#include <glm/vec3.hpp>


namespace Renderer {
	class VulkanTexture;
	class Texture;

	class Material {

    public:
        Material() = default;
        virtual ~Material() = default;
        virtual void Bind() = 0;
        void InitMaterialData(const Asset::MaterialData&& other);
        void InitMaterialData(const Asset::MaterialData& other);
    	virtual void SetAlbedoTexture(std::shared_ptr<Texture> texture);
        virtual void SetAOTexture(std::shared_ptr<Texture> texture);
        virtual void SetNormalTexture(std::shared_ptr<Texture> texture);
        virtual void SetMetalRoughnessTexture(std::shared_ptr<Texture> texture);
        virtual void SetMetalicTexture(std::shared_ptr<Texture> texture);
        virtual void SetRoughSmoothnessTexture(std::shared_ptr<Texture> texture);
        virtual void SetEmissiveTexture(std::shared_ptr<Texture> texture);
        Asset::MaterialData* GetMaterialData();
        bool has_albedo_texture = false;
        bool has_ao_texture = false;
        bool has_normal_texture = false;
        bool has_metalroughness_texture = false;

    protected:
        std::shared_ptr<Texture> texture_albedo;
        std::shared_ptr<Texture> texture_ao;
        std::shared_ptr<Texture> texture_normal;
        std::shared_ptr<Texture> texture_metalroughness;
        std::shared_ptr<Texture> texture_metallic;
        std::shared_ptr<Texture> texture_rough_smoothness;
        std::shared_ptr<Texture> texture_emissive;

        Asset::MaterialData data;
        Asset::Image* image_albedo = nullptr;
        Asset::Image* image_emissive = nullptr;
        Asset::Image* image_metalRoughness = nullptr;
        Asset::Image* image_normal = nullptr;
    };

    inline void Material::InitMaterialData(const Asset::MaterialData&& other)
    {
        data = other;
    }

    inline void Material::InitMaterialData(const Asset::MaterialData& other)
    {
        data = other;
    }

    inline void Material::SetAlbedoTexture(std::shared_ptr<Texture> texture)
    {
	    texture_albedo = texture;
    }

    inline void Material::SetAOTexture(std::shared_ptr<Texture> texture)
    {
        texture_ao = texture;
    }

    inline void Material::SetNormalTexture(std::shared_ptr<Texture> texture)
    {
        texture_normal = texture;
    }

    inline void Material::SetMetalRoughnessTexture(std::shared_ptr<Texture> texture)
    {
        texture_metalroughness = texture;
    }

    inline void Material::SetMetalicTexture(std::shared_ptr<Texture> texture)
    {
        texture_metallic = texture;
    }

    inline void Material::SetRoughSmoothnessTexture(std::shared_ptr<Texture> texture)
    {
        texture_rough_smoothness = texture;
    }

    inline void Material::SetEmissiveTexture(std::shared_ptr<Texture> texture)
    {
        texture_emissive = texture;
    }

    inline Asset::MaterialData* Material::GetMaterialData() 
    {
        return &data;
    }
}
#endif // !MATERIAL_H
