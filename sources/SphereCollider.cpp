//
// Created by lovri on 9/6/2024.
//
#include "SphereCollider.h"
#include "glm/geometric.hpp"

SphereCollider::SphereCollider(glm::vec3 position, float radius, float frictionCoefficient) :
        position(position),
        radius(radius),
        frictionCoefficient(frictionCoefficient) {

}

glm::vec3 SphereCollider::Expel(glm::vec3 &particleNextPosition, const glm::vec3 &particleVelocity) const {
    if (glm::distance(particleNextPosition, position) > radius) {
        return glm::vec3(0);
    }
    glm::vec3 normal = glm::normalize(particleNextPosition - position);

    particleNextPosition = position + normal * radius;

    glm::vec3 tangentialVelocity = particleVelocity - glm::dot(particleVelocity, normal) * normal;
    return -frictionCoefficient * tangentialVelocity;
}

