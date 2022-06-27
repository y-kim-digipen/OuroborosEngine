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
    	virtual void SetTexture(std::shared_ptr<Texture> texture);
        Asset::MaterialData* GetMaterialData();
        bool has_texture = false;
    protected:
        std::shared_ptr<Texture> texture_;
        Asset::MaterialData data;
        Asset::Image* image_ = nullptr;
    };

    inline void Material::InitMaterialData(const Asset::MaterialData&& other)
    {
        data = other;
    }

    inline void Material::SetTexture(std::shared_ptr<Texture> texture)
    {
	    texture_ = texture;
    }

    inline Asset::MaterialData* Material::GetMaterialData() 
    {
        return &data;
    }
}
#endif // !MATERIAL_H
