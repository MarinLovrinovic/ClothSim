// Local Headers

// System Headers
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Shader.h"
#include "stb_image.h"
#include "TriangleMesh.h"
//#include "assimp/material.h"
//#include "assimp/types.h"
//#include "assimp/Importer.hpp"
//#include "assimp/scene.h"
//#include "assimp/postprocess.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>


// Standard Headers
#include <cstdio>
#include <cstdlib>

#include <iostream>


using namespace std;

int width = 900, height = 600;

//na temelju naziva sjencara izradi objekt sjencara. Npr. shader0 -> shader0.vert i shader0.frag
Shader* LoadShaderWithGeometry(char* path, char* naziv) {
    std::string sPath(path);
    std::string pathVert;
    std::string pathFrag;

    //malo je nespretno napravljeno jer ne koristimo biblioteku iz c++17, a treba podrzati i windows i linux

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
    objPath.append("\\glava\\glava.obj"); //za linux pretvoriti u forwardslash

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


void framebuffer_size_callback(GLFWwindow * window, int Width, int Height)
{
    width = Width;
    height = Height;
}


/*****************************************************************************************************************************
organizacija koda:
Svaki primjer ima dva dijela. Dio koji se izvrsi prije pokretanja glavne petlje i dio koji se izvrsava unutar glavne petlje.
Generirani VAO, VBO i EBO se izmedu nekih primjera dijele.

ako se ne pokrece!!
primjer 4b koristi funkcionalnosti iz novijih verzija OpenGL, pa ako ga zakomentirate mozda popravi vas problem
*****************************************************************************************************************************/

int main(int argc, char * argv[]) {

    GLFWwindow* window;


    std::cout << argv[0] << std::endl;

    glfwInit();

    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(width, height, "ClothSim", nullptr, nullptr);

    // Check for Valid Context
    if (window == nullptr) {
        fprintf(stderr, "Failed to Create OpenGL Context");
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    gladLoadGL();

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); //funkcija koja se poziva prilikom mijenjanja velicine prozora

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    fprintf(stderr, "OpenGL %s\n", glGetString(GL_VERSION));

    glClearColor(0.15, 0.1, 0.1, 1);

    //generiranje buffera
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    Shader *sjencar;

    //primjer 1
    //vrhovi kroz buffer, a boja kroz uniformnu varijablu

    sjencar = LoadShaderWithGeometry(argv[0], "shader0");

    GLint lokacijaUniformVarijable = glGetUniformLocation(sjencar->ID, "u_color");

    float trokutKoordinate[9] = {
            //  koordinate
            -1, -1, 0,
            1, -1, 0,
            0,  1, 0 };

    glBindVertexArray(VAO); //Iduce naredbe se odnose na vezani(bind) VAO sve dok se ne promijeni ili iskljuci.

    glBindBuffer(GL_ARRAY_BUFFER, VBO); //Iduce naredbe se odnose na vezani(bind) VBO sve dok se ne promijeni ili iskljuci
    glBufferData(GL_ARRAY_BUFFER, sizeof(trokutKoordinate), trokutKoordinate, GL_STATIC_DRAW);  //naredba za definiranje gdje se nalaze podaci u glavnoj memoriji za prijenos na graf. karticu

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); //pogledati shader0.vert
    glEnableVertexAttribArray(0); //pogledati shader0.vert

    glBindVertexArray(0);

    // ucitaj kocku
    auto* triangleMesh = ImportMesh(string(argv[0]));
    triangleMesh->Normalize();

    triangleMesh->SendToGpu();

    //glavna petlja programa
    while (glfwWindowShouldClose(window) == false) {

        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        /********************************************************/
        //primjer 1
        glUseProgram(sjencar->ID); //koristi sjencar shader0
        glUniform3f(lokacijaUniformVarijable, 0.5, 0.5, 1.0); //pogledaj shader0.vert

        glViewport(0, height/2, width/3, height/2); //u koji dio okvira prozora se iscrtava (gore lijevo)

        glBindVertexArray(VAO); //koristi VAO[0] za crtanje
        glDrawArrays(GL_TRIANGLES, 0, 3); //poziv crtanja
        glBindVertexArray(0);

        glViewport(10, 10, width - 20, height - 20); //u koji dio okvira prozora se iscrtava (gore lijevo)

        //nacrtaj kocku
        triangleMesh->Bind();
        triangleMesh->Draw();

        glfwSwapBuffers(window);
        glfwPollEvents();

    }
    delete sjencar;

    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    glfwTerminate();

    return EXIT_SUCCESS;
}