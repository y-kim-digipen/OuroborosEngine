#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

#include "buffer_data_type.h"

namespace Renderer {
    class Material {

    public:
        Material() = default;
        virtual ~Material() = default;
        virtual void Bind() = 0;
        void InitMaterialData(const MaterialData&& other);
        MaterialData* GetMaterialData();
    protected:
        MaterialData data;
    };

    inline void Material::InitMaterialData(const MaterialData&& other)
    {
        data = other;
    }

    inline MaterialData* Material::GetMaterialData() 
    {
        return &data;
    }
}
#endif // !MATERIAL_H
