
#ifndef CLOTHSIM_CLOTH_H
#define CLOTHSIM_CLOTH_H

#include <vector>
#include <map>
#include "glm/vec3.hpp"
#include "Renderable.h"
#include "TriangleMesh.h"
#include "Object.h"
#include "SphereCollider.h"

using namespace std;

class Cloth {
private:
    int dim;
    vector<vector<glm::vec3>> previousPos;
    vector<vector<glm::vec3>> currentPos;
    vector<vector<glm::vec3>> nextPos;
    vector<vector<glm::vec3>> currentNormals;
    vector<vector<glm::vec3>> currentVelocities;
    vector<vector<glm::vec3>> currentFrictions;
    float springConstant;
    float springDampingCoefficient;
    float dragCoefficient;

    float equilibriumDistance;

    TriangleMesh* mesh;
public:
    Object* object;

    Cloth(int dimension, float springConstant, float springDampingCoefficient, float dragCoefficient, Shader* shader, glm::vec3 position, float sideLength);
    void Update(float dt, glm::vec3 gravity, glm::vec3 airflow, const vector<Collider*>& colliders);
};

#endif //CLOTHSIM_CLOTH_H
