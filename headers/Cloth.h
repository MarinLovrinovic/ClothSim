
#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

#include <vector>
#include "glm/vec3.hpp"
#include "Renderable.h"
#include "TriangleMesh.h"
#include "Object.h"

using namespace std;

class Cloth : Renderable {
    int dim;
    vector<vector<glm::vec3>> previousPos;
    vector<vector<glm::vec3>> currentPos;
    vector<vector<glm::vec3>> nextPos;
    float springConstant;
    float equilibriumDistance;
    float diagonalEquilibriumDistance;

    TriangleMesh* mesh;
public:
    Object* object;

    Cloth(int dimension, float springConstant, float equilibriumDistance, Shader* shader);
    void Update(float dt);
};

#endif //CLOTHSIM_CLOTH_H
