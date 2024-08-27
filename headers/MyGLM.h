#ifndef IRGLAB_MYGLM_H
#define IRGLAB_MYGLM_H

#include "glm/glm.hpp"

class MyGLM {
public:
    static glm::mat4 Translate3D(glm::vec3 translateVector);
    static glm::mat4 Scale3D(glm::vec3 scaleVector);
    static glm::mat4 Rotate3D(float angle, glm::vec3 axis);
    static glm::mat4 LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp);
    static glm::mat4 Frustum(float l, float r, float b, float t, float n, float f );
};

#endif //IRGLAB_MYGLM_H
