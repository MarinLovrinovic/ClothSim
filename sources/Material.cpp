#include "Material.h"
#include "Shader.h"


Material::Material(Shader *shader) {
    SetShader(shader);
}

void Material::BeforeDraw() const {
    glUniform3f(materialAmbientLocation, ambient.x, ambient.y, ambient.z);
    glUniform3f(materialDiffuseLocation, diffuse.x, diffuse.y, diffuse.z);
    glUniform3f(materialSpecularLocation, specular.x, specular.y, specular.z);
    glUniform1f(materialShininessLocation, shininess);
}

Material::Material() = default;

void Material::SetShader(Shader *shader) {
    materialAmbientLocation = glGetUniformLocation(shader->ID, "materialAmbient");
    materialDiffuseLocation = glGetUniformLocation(shader->ID, "materialDiffuse");
    materialSpecularLocation = glGetUniformLocation(shader->ID, "materialSpecular");
    materialShininessLocation = glGetUniformLocation(shader->ID, "materialShininess");
}

