#ifndef MATERIAL_H
#define MATERIAL_H

#include "../../common/assets.h"
#include <glm/vec3.hpp>


namespace Renderer {
    class Material {

    public:
        Material() = default;
        virtual ~Material() = default;
        virtual void Bind() = 0;
        void InitMaterialData(const Asset::MaterialData&& other);
        Asset::MaterialData* GetMaterialData();
    protected:
        Asset::MaterialData data;
    };

    inline void Material::InitMaterialData(const Asset::MaterialData&& other)
    {
        data = other;
    }

    inline Asset::MaterialData* Material::GetMaterialData() 
    {
        return &data;
    }
}
#endif // !MATERIAL_H
