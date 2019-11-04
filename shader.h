#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "camera.h"
#include "assman.h"

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
    glm::vec4 diffuse;
    glm::vec4 specular;

    float shininess;

    static Material Default();
    static Material DebugLight();
    static Material Hand();
};

class Shader
{
private:
    unsigned int ID;

    unsigned int uModelMatrix; // One of the worst offenders
    unsigned int uViewMatrix;
    unsigned int uProjectionMatrix;

    void setup_handles();
    void checkCompileErrors(GLuint shader, std::string type);
    void setup(const char* vertexPath, const char* fragmentPath);
public:
    static Shader FromPath(const char* vertexPath, const char* fragmentPath);

    void use();

    void setCamera(const Camera& camera) const;

    void setModelMatrix(const glm::mat4 &model_matrix) const;
    void setDirectionalLight(int number, const DirectionalLight& light) const;

    void setPointLight(int number, const PointLight& light) const;

    void setMaterial(const std::string& name, const Material& material) const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;

    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec2(const std::string& name, float x, float y) const;

    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;

    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setVec4(const std::string& name, float x, float y, float z, float w);

    void setMat2(const std::string& name, const glm::mat2& mat) const;
    void setMat3(const std::string& name, const glm::mat3& mat) const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;

};
#endif
