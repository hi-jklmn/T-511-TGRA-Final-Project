#include <glm/glm.hpp>
#include <GL/glew.h>

class Debug {
    GLuint line_vao;
    GLuint line_vbo;

    Model light_debug;

    void setup_models() {
        Mesh light_sphere = Mesh::Sphere();
        light_sphere.material = Material::DebugLight();
        light_debug = Model::FromMesh(light_sphere);
    }

    void setup_buffers() {
        glGenVertexArrays(1, &line_vao);
        glGenBuffers(1, &line_vbo);

        glBindVertexArray(line_vao);
        glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        glBufferData(GL_ARRAY_BUFFER, 2 * 3 * sizeof(float), NULL, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

public:
    static Debug init() {
        Debug d;
        d.setup_buffers();
        d.setup_models();
        return d;
    }
    void draw_debug_sphere(Shader shader, glm::vec3 p) {
        Entity e_sphere = Entity::FromModel(&light_debug);
        e_sphere.setScale(glm::vec3(0.2f));
        e_sphere.setPosition(p);
        e_sphere.draw(shader);
    }

    void draw_point_light(Shader shader, PointLight light) {
        Entity e_light = Entity::FromModel(&light_debug);
        e_light.setScale(glm::vec3(0.2f));
        e_light.setPosition(light.position);
        e_light.draw(shader);
    }

    void draw_line(Shader shader, glm::vec3 p, glm::vec3 q) {
        shader.setMat4("uModelMatrix", glm::mat4(1.0f));
        glBindVertexArray(line_vao);
        glBindBuffer(GL_ARRAY_BUFFER, line_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(p), &p);
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(p), sizeof(q), &q);
        glDrawArrays(GL_LINES, 0, 2);
        glBindVertexArray(0);
    }

    void draw_grid(Shader shader) {
        int min = -10;
        int max = 10;
        for (int i = min; i <= max; i++) {
            glm::vec3 a, b, c, d;
            a = glm::vec3(i, 0, max);
            b = glm::vec3(i, 0, min);
            c = glm::vec3(min, 0, i);
            d = glm::vec3(max, 0, i);
            draw_line(shader, a, b);
            draw_line(shader, c, d);
        }
    }
};
