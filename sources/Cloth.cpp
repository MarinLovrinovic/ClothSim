#include "Cloth.h"
#include "TriangleMesh.h"
#include <glm/glm.hpp>


void CalcVertices(const vector<vector<glm::vec3>>& currentPositions, vector<glm::vec3>& vertices) {
    int dim = currentPositions.size();
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            vertices[dim * i + j] = currentPositions[i][j];
        }
    }
}

template<class T>
void TwoDimToOneDim(const vector<vector<T>>& twoDim, vector<T>& oneDim) {
    int dim = twoDim.size();
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            oneDim[dim * i + j] = twoDim[i][j];
        }
    }
}

void PrepareVertices(const vector<vector<glm::vec3>>& currentPositions, vector<glm::vec3>& vertices) {
    int dim = currentPositions.size();
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            vertices[dim * i + j] = currentPositions[i][j];
        }
    }
}

void CalcNormals(const vector<vector<glm::vec3>>& currentPositions, vector<glm::vec3>& normals) {
    int dim = currentPositions.size();
    for (glm::vec3& normal : normals) {
        normal = glm::vec3(0);
    }
    for (int i = 0; i < dim - 1; ++i) {
        for (int j = 0; j < dim - 1; ++j) {

            glm::vec3 faceNormal1 =
                    glm::normalize(glm::cross(currentPositions[i][j + 1] - currentPositions[i][j],
                                              currentPositions[i + 1][j] - currentPositions[i][j]));
            normals[i * dim + j] += faceNormal1;
            normals[i * dim + (j + 1)] += faceNormal1;
            normals[(i + 1) * dim + j] += faceNormal1;

            glm::vec3 faceNormal2 =
                    glm::normalize(glm::cross(currentPositions[i + 1][j] - currentPositions[i + 1][j + 1],
                                              currentPositions[i][j + 1] - currentPositions[i + 1][j + 1]));
            normals[i * dim + (j + 1)] += faceNormal2;
            normals[(i + 1) * dim + (j + 1)] += faceNormal2;
            normals[(i + 1) * dim + j] += faceNormal2;
        }
    }
    for (glm::vec3& normal: normals) {
        normal = glm::normalize(normal);
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

Cloth::Cloth(int dimension, float springConstant, Shader* shader, glm::vec3 position, float sideLength) :
dim(dimension),
springConstant(springConstant) {
    equilibriumDistance = sideLength / (dimension - 1);
    diagonalEquilibriumDistance = equilibriumDistance * sqrt(2);
    previousPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    nextPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentNormals = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentVelocities = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentFrictions = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));

    glm::vec3 corner = position + glm::vec3(-0.5f * sideLength, 0, -0.5f * sideLength);
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            glm::vec3 particlePos = corner + glm::vec3(i * equilibriumDistance, 0, j * equilibriumDistance);
            currentPos[i][j] = particlePos;
            previousPos[i][j] = particlePos;
            currentVelocities[i][j] = glm::vec3(0);
            currentFrictions[i][j] = glm::vec3(0);
        }
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

typedef struct {
    int i;
    int j;
} NeighborOffset;

void Cloth::Update(float dt, glm::vec3 gravity, glm::vec3 airflow) {
    vector<NeighborOffset> neighborOffsets = {
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
        equilibriumDistances[n.i][n.j] = sqrt(n.i * n.i + n.j * n.j);
    }

    float springDampingCoefficient = 0.01f;

    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            glm::vec3 particlePosition = currentPos[i][j];
            glm::vec3 particleVelocity = currentVelocities[i][j];
            glm::vec3 acceleration = gravity;

            for (NeighborOffset neighborOffset : neighborOffsets) {
                int neighborI = i + neighborOffset.i;
                int neighborJ = j + neighborOffset.j;
                if (neighborI < 0 || dim <= neighborI || neighborJ < 0 || dim <= neighborJ) {
                    continue;
                }

                glm::vec3 neighborPosition = currentPos[neighborI][neighborJ];
                float unitDistancesFromParticle = equilibriumDistances[neighborOffset.i][neighborOffset.j];
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

            // air resistance
            glm::vec3 velocityInAir = particleVelocity - airflow;
            float dragCoefficient = 0.001f;
            acceleration +=
                    -dragCoefficient * abs(glm::dot(velocityInAir, currentNormals[i][j])) * velocityInAir;

            // friction
            acceleration += currentFrictions[i][j];

            glm::vec3 nextPosition = 2.0f * particlePosition - previousPos[i][j] + acceleration * dt * dt;

            glm::vec3 colliderCenter = glm::vec3(0, 0, 0);
            float colliderRadius = 1;
            float colliderFrictionCoefficient = 10.0f;

            currentFrictions[i][j] = glm::vec3(0);
            if (glm::distance(nextPosition, colliderCenter) < colliderRadius) {
                glm::vec3 normal = glm::normalize(nextPosition - colliderCenter);

                nextPosition = colliderCenter + normal * colliderRadius;

                glm::vec3 tangentialVelocity = particleVelocity - glm::dot(particleVelocity, normal) * normal;
                currentFrictions[i][j] = -colliderFrictionCoefficient * tangentialVelocity;
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