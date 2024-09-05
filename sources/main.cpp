// Local Headers


// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//nekima mozda ne radi primjerASSIMP zbog ponovnih definicija stbi funkcija.
//Jedno od mogucih rjesenja je da se zakomentira linija #define STB_IMAGE_IMPLEMENTATION.
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "TriangleMesh.h"
#include "Object.h"
#include "FPSManager.h"
#include "Renderer.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "Camera.h"
#include "glm/ext/matrix_transform.hpp"
#include "Light.h"
#include "Particle.h"
#include "Cloth.h"

#define _USE_MATH_DEFINES

// Standard Headers
#include <iostream>
#include <cstdlib>
#include <vector>

using namespace std;

const double pi = 3.14159265358979323846;

//malo je nespretno napravljeno jer ne koristimo c++17, a treba podrzati i windows i linux,
//slobodno pozivajte new Shader(...); direktno

Shader* LoadShader(char* path, char* naziv) {
    std::string sPath(path);
    std::string pathVert;
    std::string pathFrag;

    pathVert.append(path, sPath.find_last_of("\\/") + 1);
    pathFrag.append(path, sPath.find_last_of("\\/") + 1);
    if (pathFrag[pathFrag.size() - 1] == '/') {
        pathVert.append("shaders/");
        pathFrag.append("shaders/");
    }
    else if (pathFrag[pathFrag.size() - 1] == '\\') {
        pathVert.append("shaders\\");
        pathFrag.append("shaders\\");
    }
    else {
        std::cerr << "nepoznat format pozicije shadera";
        exit(1);
    }

    pathVert.append(naziv);
    pathVert.append(".vert");
    pathFrag.append(naziv);
    pathFrag.append(".frag");

    return new Shader(pathVert.c_str(), pathFrag.c_str());
}


TriangleMesh* ImportMesh(const string& path) {
    Assimp::Importer importer;

    std::string dirPath(path, 0, path.find_last_of("\\/"));
    std::string resPath(dirPath);
    resPath.append("\\resources"); //za linux pretvoriti u forwardslash
    std::string objPath(resPath);
    objPath.append("\\sphere\\sphere.obj"); //za linux pretvoriti u forwardslash

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType |
                                             aiProcess_FlipUVs |
                                             aiProcess_GenNormals
    );
    if (!scene) {
        cerr << importer.GetErrorString();
        exit(-1);
    }
    if (!scene->HasMeshes()){
        cerr << "Scene has no meshes." << endl;
        exit(-1);
    }
    aiMesh* mesh = scene->mMeshes[0];

    vector<glm::vec3> vertices;
    for (int i = 0; i < mesh->mNumVertices; i++) {
        auto vertex = mesh->mVertices[i];
        vertices.emplace_back(vertex.x, vertex.y, vertex.z);
    }

    vector<glm::vec3> normals;
    if (mesh->mNormals != nullptr){
        for (int i = 0; i < mesh->mNumVertices; i++) {
            auto normal = mesh->mNormals[i];
            normals.emplace_back(normal.x, normal.y, normal.z);
        }
    }

    vector<int> indices;
    for (int i = 0; i < mesh->mNumFaces; i++) {
        auto face = mesh->mFaces[i];
        if (face.mNumIndices != 3) {
            std::cerr << "Error: Only triangular faces are supported" << std::endl;
            exit(-1);
        }
        for (int j = 0; j < face.mNumIndices; j++){
            indices.emplace_back(face.mIndices[j]);
        }
    }

    return new TriangleMesh(vertices, normals, indices);
}

Material* ImportMaterial(const string& path) {
    Assimp::Importer importer;

    std::string dirPath(path, 0, path.find_last_of("\\/"));
    std::string resPath(dirPath);
    resPath.append("\\resources"); //za linux pretvoriti u forwardslash
    std::string objPath(resPath);
    objPath.append("\\sphere\\sphere.obj"); //za linux pretvoriti u forwardslash

    const aiScene* scene = importer.ReadFile(objPath.c_str(),
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType |
                                             aiProcess_FlipUVs |
                                             aiProcess_GenNormals

    );

    if (!scene) {
        cerr << importer.GetErrorString();
        exit(-1);
    }
    if (!scene->HasMaterials()) {
        cerr << "Scene has no materials." << endl;
        exit(-1);
    }
    aiMaterial* material = scene->mMaterials[scene->mNumMaterials - 1];

    aiColor3D ambientK, diffuseK, specularK;
    auto* output = new Material;

    material->Get(AI_MATKEY_COLOR_AMBIENT, ambientK);
    output->ambient = glm::vec3(ambientK.r, ambientK.g, ambientK.g);

    material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseK);
    output->diffuse = glm::vec3(diffuseK.r, diffuseK.g, diffuseK.g);

    material->Get(AI_MATKEY_COLOR_SPECULAR, specularK);
    output->specular = glm::vec3(specularK.r, specularK.g, specularK.g);

    material->Get(AI_MATKEY_SHININESS, output->shininess);
    return output;
}

Renderer* renderer;

void FramebufferSizeCallback(GLFWwindow* window, int Width, int Height)
{
    renderer->width = Width;
    renderer->height = Height;
    glViewport(0, 0, Width, Height);
}

Camera camera;
double pitch;
double yaw;

void CursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    glm::vec2 newPos(xpos, ypos);
    auto delta = newPos - glm::vec2(renderer->width * 0.5, renderer->height * 0.5);
    delta *= -0.001;

    pitch += delta.y;
    pitch = std::max(-pi * 0.5 + 0.01, std::min(pitch, pi * 0.5 - 0.01));

    yaw += delta.x;

    camera.SetRotation(yaw, glm::vec3(0, 1, 0));
    camera.Rotate(pitch, camera.LocalToGlobalDir() * glm::vec4(1, 0, 0, 0));
}

bool forwardPressed;
bool backPressed;
bool leftPressed;
bool rightPressed;
bool upPressed;
bool downPressed;

glm::vec3 moveVector;

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_REPEAT) return;

    bool pressed = action == GLFW_PRESS;
    if (key == GLFW_KEY_W) {
        forwardPressed = pressed;
    } else if (key == GLFW_KEY_A) {
        leftPressed = pressed;
    } else if (key == GLFW_KEY_S) {
        backPressed = pressed;
    } else if (key == GLFW_KEY_D) {
        rightPressed = pressed;
    } else if (key == GLFW_KEY_E) {
        downPressed = pressed;
    } else if (key == GLFW_KEY_Q) {
        upPressed = pressed;
    } else {
        return;
    }

    moveVector = glm::vec3((rightPressed ? 1.0f : 0.0f) + (leftPressed ? -1.0f : 0.0f),
                           (upPressed ? 1.0f : 0.0f) + (downPressed ? -1.0f : 0.0f),
                           (backPressed ? 1.0f : 0.0f) + (forwardPressed ? -1.0f : 0.0f));
}

int main(int argc, char* argv[]) {
    renderer = new Renderer(1000, 1000);

    glfwSetFramebufferSizeCallback(renderer->window, FramebufferSizeCallback); //funkcija koja se poziva prilikom mijenjanja velicine prozora
    glfwSetCursorPosCallback(renderer->window, CursorPosCallback);
    glfwSetKeyCallback(renderer->window, KeyCallback);
    glfwSetInputMode(renderer->window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    auto* triangleMesh = ImportMesh(string(argv[0]));
    triangleMesh->Normalize();

//    auto* object = new Object(triangleMesh, LoadShader(argv[0], "scene"));
//    object->SendToGpu();
//    object->material = ImportMaterial(string(argv[0]));
//    renderer->RegisterRenderable(object);

    Shader* shader = LoadShader(argv[0], "scene");
    auto* collider = new Object(triangleMesh, shader);
    collider->SendToGpu();
    collider->transforms[0].SetScale(glm::vec3(0.93));
    collider->material = ImportMaterial(string(argv[0]));
    collider->material->SetShader(shader);
    renderer->RegisterRenderable(collider);

//    auto* object2 = new Object(triangleMesh, LoadShader(argv[0], "scene"));
//    object2->SendToGpu();
//    Transform transform;
//    transform.SetPosition(glm::vec3(3, 0, 0));
//    transform.SetRotation(1.5, glm::normalize(glm::vec3(1, 1, 1)));
//    transform.SetScale(glm::vec3(3, 2, 1));
//    object2->transforms[0] = transform;
//    object2->material->diffuse = glm::vec3(1, 0, 0);
//    object2->material->specular = glm::vec3(0, 0, 1);
//    renderer->RegisterRenderable(object2);

    camera.SetPosition(glm::vec3(0, 0, 5));

    Light light;
    light.SetPosition(glm::vec3(10, 10, 10));

//    Cloth cloth(30, 20, LoadShader(argv[0], "scene"), glm::vec3(0, 1, 0), 4);
//    Cloth cloth(30, 15, LoadShader(argv[0], "scene"), glm::vec3(0, 1, 0), 4);

    Cloth cloth(60, 1000, LoadShader(argv[0], "scene"), glm::vec3(0, 1, 0), 4);
    renderer->RegisterRenderable(cloth.object);

    float dt = 0.005f;
    while (!glfwWindowShouldClose(renderer->window)) {
        if (moveVector != glm::vec3(0.0f)) {
            camera.Move(dt * camera.LocalToGlobalDir() * glm::vec4(moveVector, 0.0f));
        }

        cloth.Update(dt, glm::vec3(0, -0.4, 0), glm::vec3(0, 0, 0));

        renderer->Render(camera, light);

        glfwPollEvents();
        if (glfwGetKey(renderer->window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(renderer->window, true);
        glfwSetCursorPos(renderer->window, renderer->width * 0.5, renderer->height * 0.5);
    }
    delete renderer;
    return EXIT_SUCCESS;
}
