//
// Created by lovri on 8/29/2024.
//

#ifndef CLOTHSIM_PARTICLE_H
#define CLOTHSIM_PARTICLE_H

#include "glm/vec3.hpp"

class Particle {
public:
    glm::vec3 position;
    glm::vec3 previousPosition;
};


#endif //CLOTHSIM_PARTICLE_H
