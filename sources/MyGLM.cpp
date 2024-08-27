
#include "MyGLM.h"

glm::mat4 MyGLM::Translate3D(glm::vec3 translateVector) {
    return {glm::vec4(1, 0, 0, 0),
            glm::vec4(0, 1, 0, 0),
            glm::vec4(0, 0, 1, 0),
            glm::vec4(translateVector, 1)};
}

glm::mat4 MyGLM::Scale3D(glm::vec3 scaleVector) {
    return {scaleVector.x, 0, 0, 0,
            0, scaleVector.y, 0, 0,
            0, 0, scaleVector.z, 0,
            0, 0, 0, 1};
}

// translates the simple transformation matrix of rotating around z
// from an orthogonal coordinate system where the z basis vector is equal to axis
// to the global coordinate system
glm::mat4 MyGLM::Rotate3D(float angle, glm::vec3 axis) {
    glm::vec3 arbitraryVector(1, 0, 0);
    // if the vectors are colinear, need a new arbitrary vector
    if (axis.y == 0 && axis.z == 0) {
        arbitraryVector = glm::vec3(0, 1, 0);
    }
    glm::vec3 z = axis;
    glm::vec3 x = glm::cross(z, arbitraryVector);
    glm::vec3 y = glm::cross(z, x);

    glm::mat4 localToGlobal = glm::mat4(glm::vec4(x, 0),
                                        glm::vec4(y, 0),
                                        glm::vec4(z, 0),
                                        glm::vec4(0, 0, 0, 1));

    glm::mat4 globalToLocal = glm::inverse(localToGlobal);

    glm::mat4 rotateAroundZAxis(cos(angle), sin(angle), 0, 0,
                                -sin(angle), cos(angle), 0, 0,
                                0, 0, 1, 0,
                                0, 0, 0, 1);

    return localToGlobal * rotateAroundZAxis * globalToLocal;
}

glm::mat4 MyGLM::LookAt(glm::vec3 eye, glm::vec3 center, glm::vec3 viewUp) {
    auto viewNegZ = glm::normalize(center - eye);
    auto viewX = glm::normalize(glm::cross(viewNegZ, viewUp));
    auto viewY = glm::cross(viewX, viewNegZ);

    glm::mat4 rotation(glm::vec4(viewX, 0),
                       glm::vec4(viewY, 0),
                       glm::vec4(-viewNegZ, 0),
                       glm::vec4(0, 0, 0, 1));

    glm::mat4 translation = Translate3D(center);

    glm::mat4 localToGlobal = translation * rotation;
    return glm::inverse(localToGlobal);
}

glm::mat4 MyGLM::Frustum(float l, float r, float b, float t, float n, float f) {
    return {(2 * n) / (r - l), 0,                  0,                      0,
            0,                 (2 * n) / (t - b), 0,                      0,
            (r + l) / (r - l), (t + b) / (t - b), -(f + n) / (f - n),     -1,
            0,                 0,                 (-2 * f * n) / (f - n), 0};
}
