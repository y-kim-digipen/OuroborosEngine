#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm/vec3.hpp>

class Material {

public:
    Material() = default;
    virtual ~Material() = default;

    virtual void Bind() = 0;

protected:
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float shininess;
};


#endif // !MATERIAL_H
