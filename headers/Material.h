#ifndef IRGLAB_MATERIAL_H
#define IRGLAB_MATERIAL_H

#include "glm/glm.hpp"
#include "glad/glad.h"
#include "Shader.h"

class Material {
public:
    glm::vec3 ambient = glm::vec3(0.1, 0.1, 0.1);
    glm::vec3 diffuse = glm::vec3(1, 1, 1);
    glm::vec3 specular = glm::vec3(0.3, 0.3, 0.3);
    float shininess = 30;

    GLint materialAmbientLocation;
    GLint materialDiffuseLocation;
    GLint materialSpecularLocation;
    GLint materialShininessLocation;

    explicit Material(Shader *shader);
    Material();
    void SetShader(Shader* shader);
    void BeforeDraw() const;
};

#endif //IRGLAB_MATERIAL_H
