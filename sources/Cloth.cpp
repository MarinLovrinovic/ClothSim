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

vector<int> CalcIndices(int dim){
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

Cloth::Cloth(int dimension, float springConstant, float equilibriumDistance, Shader* shader) :
dim(dimension),
springConstant(springConstant),
equilibriumDistance(equilibriumDistance) {
    diagonalEquilibriumDistance = equilibriumDistance * sqrt(2);
    previousPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    currentPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));
    nextPos = vector<vector<glm::vec3>>(dim, vector<glm::vec3>(dim));

    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            glm::vec3 particlePos = glm::vec3(-1.45f + i * equilibriumDistance, 1.05, -1.45f + j * equilibriumDistance);
            currentPos[i][j] = particlePos;
            previousPos[i][j] = particlePos;
        }
    }

    vector<glm::vec3> vertices(dim * dim);
    CalcVertices(currentPos, vertices);
    vector<int> indices = CalcIndices(currentPos.size()); // always the same?
    vector<glm::vec3> normals(dim * dim);
    CalcNormals(currentPos, normals);

    mesh = new TriangleMesh(vertices, normals, indices);
    object = new Object(mesh, shader);
}

void Cloth::Update(float dt) {
    for (int i = 0; i < dim; ++i) {
        for (int j = 0; j < dim; ++j) {
            vector<glm::vec3> neighbors;
            if (0 < i) {
                neighbors.push_back(currentPos[i - 1][j]);
            } if (i < dim - 1) {
                neighbors.push_back(currentPos[i + 1][j]);
            } if (0 < j) {
                neighbors.push_back(currentPos[i][j - 1]);
            } if (j < dim - 1) {
                neighbors.push_back(currentPos[i][j + 1]);
            }

            glm::vec3 acceleration = glm::vec3(0, -0.1, 0);
            glm::vec3 particlePosition = currentPos[i][j];
            for (auto neighborPosition : neighbors) {
                float neighborDistance = glm::distance(particlePosition, neighborPosition);
                float dx = neighborDistance - equilibriumDistance;
                acceleration += (neighborPosition - particlePosition) / neighborDistance * dx * springConstant;
            }

            vector<glm::vec3> diagonalNeighbors;
            if (0 < i && 0 < j) {
                diagonalNeighbors.push_back(currentPos[i - 1][j - 1]);
            } if (i < dim - 1 && j < dim - 1) {
                diagonalNeighbors.push_back(currentPos[i + 1][j + 1]);
            } if (i < dim - 1 && 0 < j) {
                diagonalNeighbors.push_back(currentPos[i + 1][j - 1]);
            } if (0 < i && j < dim - 1) {
                diagonalNeighbors.push_back(currentPos[i - 1][j + 1]);
            }

            for (auto neighborPosition : diagonalNeighbors) {
                float neighborDistance = glm::distance(particlePosition, neighborPosition);
                float dx = neighborDistance - diagonalEquilibriumDistance;
                acceleration += (neighborPosition - particlePosition) / neighborDistance * dx * springConstant;
            }


            glm::vec3 nextPosition = 2.0f * particlePosition - previousPos[i][j] + acceleration * dt * dt;

            glm::vec3 colliderCenter = glm::vec3(0, 0, 0);
            float colliderRadius = 1;

            if (glm::distance(nextPosition, colliderCenter) < colliderRadius) {
                nextPosition = colliderCenter + glm::normalize(nextPosition - colliderCenter) * colliderRadius;
            }
            nextPos[i][j] = nextPosition;
        }
    }
    std::swap(previousPos, currentPos);
    std::swap(currentPos, nextPos);

    CalcVertices(currentPos, mesh->vertices);
    CalcNormals(currentPos, mesh->normals);

    mesh->SendToGpu();
}