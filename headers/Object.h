#ifndef IRGLAB_OBJECT_H
#define IRGLAB_OBJECT_H

#include "TriangleMesh.h"
#include "Shader.h"
#include "Transform.h"
#include "Renderable.h"
#include "Light.h"
#include "Material.h"

class Object {
private:
    Renderable* renderable;
    Shader* shader;

    GLint modelMatrixLocation;
    GLint viewProjectionMatrixLocation;

    GLint eyePositionLocation;

    GLint lightPositionLocation;
    GLint lightAmbientLocation;
    GLint lightDiffuseLocation;
    GLint lightSpecularLocation;

    GLint materialAmbientLocation;
    GLint materialDiffuseLocation;
    GLint materialSpecularLocation;
    GLint materialShininessLocation;
public:
    Object(Renderable* renderable, Shader* shader);
    ~Object();
    vector<Transform> transforms;
    Material* material = new Material;
    void SendToGpu();
    void Draw(glm::mat4 viewProjectionMatrix, glm::vec3 eyePosition, const Light &light);
};

#endif //IRGLAB_OBJECT_H
