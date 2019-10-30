#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "camera.h"

#define MAX_NR_OF_POINT_LIGHTS 5

// Heavily influenced by and in part lifted from learnopengl.com
struct DirectionalLight {
  int is_lit;
  glm::vec4 direction;

  glm::vec4 ambient;
  glm::vec4 color;

  static DirectionalLight Default();
};

struct PointLight {
  bool is_lit;
  float radius;

  glm::vec4 position;
  glm::vec4 color;

  static PointLight Default();
};

struct Material {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;

  float shininess;
  float emissive;

  static Material Default();
  static Material DebugLight();
};

class Shader
{
public:
    unsigned int ID;
    static Shader FromPath(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);

    void use();

    void setCamera(Camera camera) {
      setMat4("uProjectionMatrix", camera.getProjectionMatrix());
      setMat4("uViewMatrix",       camera.getViewMatrix());
      setVec4("uEyePosition",      glm::vec4(camera.position, 1.0));
    }
    void setDirectionalLight(int number, DirectionalLight light);

    void setPointLight(int number, PointLight light);

    void setMaterial(const std::string &name, Material material) const;

    void setBool(const std::string &name, bool value) const;
    void setInt(const std::string &name, int value) const;
    void setFloat(const std::string &name, float value) const;

    void setVec2(const std::string &name, const glm::vec2 &value) const;
    void setVec2(const std::string &name, float x, float y) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;
    void setVec3(const std::string &name, float x, float y, float z) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;
    void setVec4(const std::string &name, float x, float y, float z, float w);

    void setMat2(const std::string &name, const glm::mat2 &mat) const;
    void setMat3(const std::string &name, const glm::mat3 &mat) const;
    void setMat4(const std::string &name, const glm::mat4 &mat) const;

private:
    void checkCompileErrors(GLuint shader, std::string type);
    void setup(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
};
#endif