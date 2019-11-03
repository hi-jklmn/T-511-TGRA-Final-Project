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

using namespace std;

unsigned int texture_from_file(const char *path, const string &directory, bool gamma);

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
  vector<Vertex> vertices;
  vector<unsigned int> indices;
  vector<Texture> textures;
  Material material;

  Mesh(
  	vector<Vertex> vertices,
    vector<unsigned int> indices,
    vector<Texture> textures
  );

  void draw(Shader shader) const;

  static Mesh Cube();
  static Mesh BadCube();
  static Mesh Sphere(int divisions = 64);
};

class Model {
private:
  string directory;
  vector<Texture> textures_loaded;

  void processNode(aiNode *node, const aiScene *scene);

  void loadModel(string path);

  vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, string typeName);

  Mesh processMesh(aiMesh *mesh, const aiScene *scene);

public:

  vector<Mesh> meshes;
  glm::mat4 transform;

  static Model FromPath(string path);
  static Model FromMesh(Mesh mesh);
  static Model FromMeshes(vector<Mesh> meshes);

  void draw(Shader shader);
};
#endif
