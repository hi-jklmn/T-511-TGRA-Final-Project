#include "shader.h"
#include "models.h"

DirectionalLight DirectionalLight::Default() {
    DirectionalLight l;
    l.is_lit = true;
    l.direction = glm::vec4(1.0, 1.0, 1.0, 0.0);
    l.ambient = glm::vec4(0.2, 0.2, 0.0, 1.0);
    l.color = glm::vec4(0.8, 0.8, 0.7, 1.0);
    return l;
}

PointLight PointLight::Default() {
    PointLight l;
    l.is_lit = true;
    l.radius = 10.0f;
    l.position = glm::vec4(2.0f);
    l.color = glm::vec4(0.8f);
    return l;
}

Material Material::Default() {
    Material m;
    m.diffuse = glm::vec4(0.8f, 0.6f, 0.0f, 1.0f);
    m.specular = glm::vec4(0.0);
    m.shininess = 32.0f;
    return m;
}

Material Material::DebugLight() {
    Material m = Material::Default();
    m.diffuse = glm::vec4(1.0);
    m.specular = glm::vec4(1.0);
    return m;
}

Material Material::Hand() {
    Material m = Material::Default();
    m.diffuse = glm::vec4(0.6, 1.0, 0.8, 1.0);
    m.specular = glm::vec4(0.0);
    return m;
}

Shader Shader::FromPath(const char* vertexPath, const char* fragmentPath)
{
    Shader s;

    s.setup(vertexPath, fragmentPath);

    return s;
}

void Shader::use()
{
    glUseProgram(ID);
}

void Shader::setCamera(const Camera& camera) const {
    glUniformMatrix4fv(uProjectionMatrix, 1, GL_FALSE, &camera.getProjectionMatrix()[0][0]);
    glUniformMatrix4fv(uViewMatrix, 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    setVec4("uEyePosition", glm::vec4(camera.position, 1.0));
}

void Shader::setModelMatrix(const glm::mat4& model_matrix) const {
    glUniformMatrix4fv(uModelMatrix, 1, GL_FALSE, &model_matrix[0][0]);
}

void Shader::setDirectionalLight(int number, const DirectionalLight& light) const {
    std::string s_light = "uDirectionalLights[" + std::to_string(number) + "]";
    setBool(s_light + ".is_lit", light.is_lit);
    setVec4(s_light + ".direction", light.direction);
    setVec4(s_light + ".ambient", light.ambient);
    setVec4(s_light + ".color", light.color);
}

void Shader::setPointLight(int number, const PointLight& light) const {
    std::string s_light = "uPointLights[" + std::to_string(number) + "]";
    setBool(s_light + ".is_lit", light.is_lit);
    setFloat(s_light + ".radius", light.radius);
    setVec4(s_light + ".position", light.position);
    setVec4(s_light + ".color", light.color);
}

void Shader::setMaterial(const std::string& name, const Material& material) const {
    // Really ugly, but used to be name + ".<parameter>" which caused this function to be 34% of CPU time
    // This uses 14% of CPU time as profiled by Visual Studio and is still mostly in concatenating strings.
    // Testing attributes this overwhelmingly to += being faster than + for some weird reason
    // Edit: The number of parameters has also dropped since, so optimization is not as critical.

    std::string temp;

    temp = name;
    temp += ".diffuse";
    setVec4(temp, material.diffuse);

    temp = name;
    temp += ".specular";
    setVec4(temp, material.specular);

    temp = name;
    temp += ".shininess";
    setFloat(temp, material.shininess);
}

void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat3(const std::string& name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}


// Been very useful to have
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    GLint success;
    GLchar infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
        }
    }
}

void Shader::setup_handles() {
    uModelMatrix      = glGetUniformLocation(ID, "uModelMatrix");
    uViewMatrix       = glGetUniformLocation(ID, "uViewMatrix");
    uProjectionMatrix = glGetUniformLocation(ID, "uProjectionMatrix");
}

void Shader::setup(const char* vertexPath, const char* fragmentPath) {
    std::string vertexCode, fragmentCode, geometryCode;
    std::ifstream vShaderFile, fShaderFile;

    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);

        std::stringstream vShaderStream, fShaderStream;

        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();
    }
    catch (std::ifstream::failure e)
    {
        fprintf(stderr, "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ");
    }

    const char* vShaderSource = vertexCode.c_str();
    const char* fShaderSource = fragmentCode.c_str();

    unsigned int vertex, fragment;

    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderSource, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderSource, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);

    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    glDeleteShader(vertex);
    glDeleteShader(fragment);

    setup_handles();
}