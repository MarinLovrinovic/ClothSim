
#ifndef CLOTHSIM_SPHERECOLLIDER_H
#define CLOTHSIM_SPHERECOLLIDER_H

#include "glm/vec3.hpp"
#include "Collider.h"

class SphereCollider : public Collider {
public:
    glm::vec3 position;
    float radius;
    float frictionCoefficient;
    SphereCollider(glm::vec3 position, float radius, float frictionCoefficient);
    glm::vec3 Expel(glm::vec3& particleNextPosition, const glm::vec3& particleVelocity) const override;
};

#endif //CLOTHSIM_SPHERECOLLIDER_H
