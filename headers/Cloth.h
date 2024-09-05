
#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

#include <vector>
#include <map>
#include "glm/vec3.hpp"
#include "Renderable.h"
#include "TriangleMesh.h"
#include "Object.h"

using namespace std;

class Cloth {
    int dim;
    vector<vector<glm::vec3>> previousPos;
    vector<vector<glm::vec3>> currentPos;
    vector<vector<glm::vec3>> nextPos;
    vector<vector<glm::vec3>> currentNormals;
    vector<vector<glm::vec3>> currentVelocities;
    vector<vector<glm::vec3>> currentFrictions;
    float springConstant;
    float equilibriumDistance;
    float diagonalEquilibriumDistance;

    TriangleMesh* mesh;
public:
    Object* object;

    Cloth(int dimension, float springConstant, Shader* shader, glm::vec3 position, float sideLength);
    void Update(float dt, glm::vec3 gravity, glm::vec3 airflow);
};

#endif //CLOTHSIM_CLOTH_H
