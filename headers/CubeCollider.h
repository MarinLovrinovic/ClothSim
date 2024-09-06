//
// Created by lovri on 9/6/2024.
//

#ifndef CLOTHSIM_CUBECOLLIDER_H
#define CLOTHSIM_CUBECOLLIDER_H

#include "Collider.h"
#include "glm/vec3.hpp"

class CubeCollider : public Collider {
public:
    glm::vec3 position;
    float sideLength;
    float frictionCoefficient;
    CubeCollider(glm::vec3 position, float sideLength, float frictionCoefficient);
    glm::vec3 Expel(glm::vec3& particleNextPosition, const glm::vec3& particleVelocity) const override;
};

#endif //CLOTHSIM_CUBECOLLIDER_H
