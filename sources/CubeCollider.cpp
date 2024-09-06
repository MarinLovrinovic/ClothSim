//
// Created by lovri on 9/6/2024.
//
#include "CubeCollider.h"
#include "glm/geometric.hpp"

CubeCollider::CubeCollider(glm::vec3 position, float sideLength, float frictionCoefficient) :
position(position),
sideLength(sideLength),
frictionCoefficient(frictionCoefficient) {

}

glm::vec3 CubeCollider::Expel(glm::vec3 &particleNextPosition, const glm::vec3 &particleVelocity) const {
    glm::vec3 fromCenter = particleNextPosition - position;
    float halfSide = sideLength / 2;

    float dx = abs(fromCenter.x);
    float dy = abs(fromCenter.y);
    float dz = abs(fromCenter.z);

    if (dx > halfSide || dy > halfSide || dz > halfSide) {
        return glm::vec3(0);
    }

    glm::vec3 normal;
    if (dy <= dx && dz <= dx) {
        normal = glm::vec3(copysign(1, fromCenter.x), 0, 0);
        particleNextPosition += normal * (halfSide - dx);
    } else if (dx <= dy && dz <= dy) {
        normal = glm::vec3(0, copysign(1, fromCenter.y), 0);
        particleNextPosition += normal * (halfSide - dy);
    } else if (dx <= dz && dy <= dz) {
        normal = glm::vec3(0, 0, copysign(1, fromCenter.z));
        particleNextPosition += normal * (halfSide - dz);
    }

    glm::vec3 tangentialVelocity = particleVelocity - glm::dot(particleVelocity, normal) * normal;
    return -frictionCoefficient * tangentialVelocity;
}

