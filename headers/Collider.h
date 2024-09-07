//
// Created by lovri on 9/6/2024.
//

#ifndef CLOTHSIM_COLLIDER_H
#define CLOTHSIM_COLLIDER_H

#include "glm/vec3.hpp"

class Collider {
public:
    virtual glm::vec3 Expel(glm::vec3& particleNextPosition, const glm::vec3& particleVelocity) const = 0;
    virtual ~Collider() = default;
};

#endif //CLOTHSIM_COLLIDER_H
