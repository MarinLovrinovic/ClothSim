#include "Cloth.h"
#include "TriangleMesh.h"
#include <glm/glm.hpp>
#include <set>
#include <utility>

template<class T>
void TwoDimToOneDim(const vector<vector<T>>& twoDim, vector<T>& oneDim) {
    int dim = twoDim.size();
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            oneDim[dim * i + j] = twoDim[i][j];
        }
    }
}

void CalculateNormals(const vector<vector<glm::vec3>>& currentPositions, vector<vector<glm::vec3>>& normals) {
    int dim = currentPositions.size();
    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {
            normals[i][j] = glm::vec3(0);
        }
    }

    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {

            glm::vec3 faceNormal1 =
                    glm::normalize(glm::cross(currentPositions[i][j + 1] - currentPositions[i][j],
                                              currentPositions[i + 1][j] - currentPositions[i][j]));
            normals[i][j] += faceNormal1;
            normals[i][j + 1] += faceNormal1;
            normals[i + 1][j] += faceNormal1;

            glm::vec3 faceNormal2 =
                    glm::normalize(glm::cross(currentPositions[i + 1][j] - currentPositions[i + 1][j + 1],
                                              currentPositions[i][j + 1] - currentPositions[i + 1][j + 1]));
            normals[i][j + 1] += faceNormal2;
            normals[i + 1][j + 1] += faceNormal2;
            normals[i + 1][j] += faceNormal2;
        }
    }

    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {
            normals[i][j] = glm::normalize(normals[i][j]);
        }
    }
}

vector<int> CalculateIndices(int dim){
    vector<int> indices;
    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {
            indices.emplace_back(i * dim + j);
            indices.emplace_back(i * dim + (j + 1));
            indices.emplace_back((i + 1) * dim + j);

            indices.emplace_back(i * dim + (j + 1));
            indices.emplace_back((i + 1) * dim + (j + 1));
            indices.emplace_back((i + 1) * dim + j);
        }
    }
    return indices;
}

vector<glm::vec3> CalculateUVs(int dim) {
    vector<glm::vec3> uvs;
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            uvs.emplace_back((float)i / (dim - 1), (float)j / (dim - 1), 0);
        }
    }
    return uvs;
}

Cloth::Cloth(int dimension, float springConstant, float springDampingCoefficient, float dragCoefficient, Shader* shader,
             glm::vec3 position, float sideLength, bool vertical, vector<glm::ivec2> fixed) :
dim(dimension),
springConstant(springConstant),
springDampingCoefficient(springDampingCoefficient),
dragCoefficient(dragCoefficient) {
    equilibriumDistance = sideLength / (dimension - 1);
    previousPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    nextPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentNormals = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentVelocities = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentFrictions = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    colliderSurfaceNormals = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    this->fixed = vector<vector<bool>>(dim, vector<bool>(dim));

    glm::vec3 corner;
    if (vertical) {
        corner = position + glm::vec3(-0.5f * sideLength, -0.5f * sideLength, 0);
    } else {
        corner = position + glm::vec3(-0.5f * sideLength, 0, -0.5f * sideLength);
    }
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            glm::vec3 particlePos;
            if (vertical) {
                particlePos = corner + glm::vec3(i * equilibriumDistance, j * equilibriumDistance, 0);
            } else {
                particlePos = corner + glm::vec3(i * equilibriumDistance, 0, j * equilibriumDistance);
            }
            currentPos[i][j] = particlePos;
            previousPos[i][j] = particlePos;
            currentVelocities[i][j] = glm::vec3(0);
            currentFrictions[i][j] = glm::vec3(0);
            colliderSurfaceNormals[i][j] = glm::vec3(0);
            this->fixed[i][j] = false;
        }
    }
    for (glm::ivec2 f : fixed) {
        this->fixed[f.x][f.y] = true;
    }

    vector<glm::vec3> meshVertices(dim * dim);
    TwoDimToOneDim(currentPos, meshVertices);

    vector<int> meshIndices = CalculateIndices(currentPos.size());

    CalculateNormals(currentPos, currentNormals);
    vector<glm::vec3> meshNormals(dim * dim);
    TwoDimToOneDim(currentNormals, meshNormals);

    vector<glm::vec3> uvs = CalculateUVs(currentPos.size());

    mesh = new TriangleMesh(meshVertices, meshNormals, meshIndices, uvs);
    mesh->SendToGpu();
    object = new Object(mesh, shader);
}

void Cloth::Update(float dt, glm::vec3 gravity, glm::vec3 airflow, const vector<Collider*>& colliders) {
    vector<glm::ivec2> neighborOffsets = {
            {-2, -1},
            {-2, 0},
            {-2, 1},
            {-1, -2},
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {-1, 2},
            {0, -2},
            {0, -1},
            {0, 1},
            {0, 2},
            {1, -2},
            {1, -1},
            {1, 0},
            {1, 1},
            {1, 2},
            {2, -1},
            {2, 0},
            {2, 1}
    };

    map<int, map<int, float>> equilibriumDistances;
    for (auto n : neighborOffsets) {
        equilibriumDistances[n.x][n.y] = sqrt(n.x * n.x + n.y * n.y);
    }

    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            if (fixed[i][j]) {
                nextPos[i][j] = currentPos[i][j];
                continue;
            }

            glm::vec3 particlePosition = currentPos[i][j];
            glm::vec3 particleVelocity = currentVelocities[i][j];
            glm::vec3 acceleration = gravity;

            for (glm::ivec2 neighborOffset : neighborOffsets) {
                int neighborI = i + neighborOffset.x;
                int neighborJ = j + neighborOffset.y;
                if (neighborI < 0 || dim <= neighborI || neighborJ < 0 || dim <= neighborJ) {
                    continue;
                }

                glm::vec3 neighborPosition = currentPos[neighborI][neighborJ];
                float unitDistancesFromParticle = equilibriumDistances[neighborOffset.x][neighborOffset.y];
                float neighborEquilibriumDistance = equilibriumDistance * unitDistancesFromParticle;

                float neighborDistance = glm::distance(particlePosition, neighborPosition);
                glm::vec3 directionToNeighbor = (neighborPosition - particlePosition) / neighborDistance;
                float dx = neighborDistance - neighborEquilibriumDistance;
                glm::vec3 springAcceleration = directionToNeighbor * dx * springConstant;

                glm::vec3 neighborVelocity = currentVelocities[neighborI][neighborJ];
                glm::vec3 relativeVelocity = particleVelocity - neighborVelocity;
                glm::vec3 springDampingAcceleration = -springDampingCoefficient
                        * glm::dot(relativeVelocity, directionToNeighbor) * directionToNeighbor;

                acceleration += (springAcceleration + springDampingAcceleration) / unitDistancesFromParticle;
            }

            // calculate air resistance
            glm::vec3 velocityInAir = particleVelocity - airflow;

            acceleration +=
                    -dragCoefficient * abs(glm::dot(velocityInAir, currentNormals[i][j])) * velocityInAir;

            // add friction
            acceleration += currentFrictions[i][j];

            glm::vec3 nextPosition = 2.0f * particlePosition - previousPos[i][j] + acceleration * dt * dt;

            // calculate collisions and friction
            currentFrictions[i][j] = glm::vec3(0);
            for (Collider *const collider : colliders) {
                currentFrictions[i][j] += collider->Expel(nextPosition, particleVelocity);
            }
            nextPos[i][j] = nextPosition;
        }
    }
    std::swap(previousPos, currentPos);
    std::swap(currentPos, nextPos);

    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            currentVelocities[i][j] = (currentPos[i][j] - previousPos[i][j]) / dt;
        }
    }

    TwoDimToOneDim(currentPos, mesh->vertices);

    CalculateNormals(currentPos, currentNormals);
    TwoDimToOneDim(currentNormals, mesh->normals);

    mesh->SendToGpu();
}