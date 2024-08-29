//
// Created by lovri on 8/29/2024.
//

#ifndef CLOTHSIM_FABRIC_H
#define CLOTHSIM_FABRIC_H

#include "glm/vec3.hpp"

class Fabric {
    glm::vec3 previousPositions[10][10];
    glm::vec3 currentPositions[10][10];
    glm::vec3 nextPositions[10][10];
    float springConstant;
    float particleDistance;

    public
};

#endif //CLOTHSIM_FABRIC_H
