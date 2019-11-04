#ifndef MODELS_H
#define MODELS_H

#include <string>
#include <vector>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <GL/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include "assman.h"
#include "shader.h"

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec3 Tangent;
    glm::vec3 Bitangent;
    glm::vec2 TexCoords;
};

class Mesh {
private:
    /*  Render data  */
    unsigned int VAO, VBO, EBO;

    GLenum draw_mode;

    /*  Functions    */
    void setupMesh();
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Texture diffuseTexture;
    Texture specularTexture;
    Texture normalTexture;

    Material material;

    Mesh(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures
    );

    void draw(Shader shader) const;

    static Mesh Cube();
    static Mesh BadCube();
    static Mesh Sphere(int divisions = 64);
};

class Model {
private:
    std::string directory;
    std::vector<Texture> textures_loaded;

    void processNode(aiNode* node, const aiScene* scene);

    void loadModel(std::string path);

    std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, Texture::Type texture_type);

    Mesh processMesh(aiMesh* mesh, const aiScene* scene);

public:

    std::vector<Mesh> meshes;
    glm::mat4 transform;

    static Model FromPath(std::string path);
    static Model FromMesh(Mesh mesh);
    static Model FromMeshes(std::vector<Mesh> meshes);

    void draw(Shader shader);
};
#endif
