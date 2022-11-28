#ifndef MATERIAL_H
#define MATERIAL_H

#include <common/assets.h>
#include <glm/vec3.hpp>
#include <array>
#include <memory>

namespace OE
{
    namespace GUI
    {
        class MaterialConfigure;
    }
}


namespace Renderer {
	class VulkanTexture;
	class Material {
        friend class OE::GUI::MaterialConfigure;
    public:
        Material() = default;
        virtual ~Material() = default;
        virtual void Bind() = 0;
        void InitMaterialData(const Asset::MaterialData&& other);
        void InitMaterialData(const Asset::MaterialData& other);
        virtual void SetTexture(Shared::PBR_TEXTURE_TYPES type, std::shared_ptr<VulkanTexture> texture);
        virtual void Cleanup();
        Asset::MaterialData* GetMaterialData();
        int& DoUseTexture(Shared::PBR_TEXTURE_TYPES type);
        std::shared_ptr<VulkanTexture> GetTexture(Shared::PBR_TEXTURE_TYPES type);

    protected:
        std::array<std::shared_ptr<VulkanTexture>, Shared::PBR_TEXTURE_TYPES::COUNT> textures;

        Asset::MaterialData data;
    };

    inline void Material::InitMaterialData(const Asset::MaterialData&& other)
    {
        data = other;
    }

    inline void Material::InitMaterialData(const Asset::MaterialData& other)
    {
        data = other;
    }

    inline void Material::SetTexture(Shared::PBR_TEXTURE_TYPES type, std::shared_ptr<VulkanTexture> texture)
    {
        textures[type] = texture;
    }

    inline void Material::Cleanup()
    {
    }

    inline Asset::MaterialData* Material::GetMaterialData() 
    {
        return &data;
    }

    inline int& Material::DoUseTexture(Shared::PBR_TEXTURE_TYPES type)
    {
        switch (type)
        {
        case Shared::ALBEDO:
            return data.has_albedo_texture;
        case Shared::NORMAL:
            return data.has_normal_texture;
        case Shared::METALLIC_ROUGHNESS:
            return data.has_metalroughness_texture;
        case Shared::METALLIC:
            return data.has_metalic_texture;
        case Shared::ROUGHNESS:
            return data.has_roughness_texture;
        case Shared::AO:
            return data.has_ao_texture;
        case Shared::EMISSIVE:
            return data.has_emissive_texture;
        default:
            assert(false);
        }
    }

    inline std::shared_ptr<VulkanTexture> Material::GetTexture(Shared::PBR_TEXTURE_TYPES type)
    {
        return textures[type];
    }
}
#endif // !MATERIAL_H
