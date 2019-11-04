#include "models.h"
#include "shader.h"
#include "geometry.h"
#include <glm/gtc/matrix_transform.hpp>

#ifndef ENTITY_H
#define ENTITY_H

class Entity {
private:
    glm::vec3 position, rotation, scale;
    glm::mat4 model_matrix;

public:
    Model* model;
    Sphere bounding_sphere;
    bool is_selected;

    static Entity FromModel(Model* model) {
        Entity e;
        e.is_selected = false;
        e.model = model;
        e.setPRS(glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f));
        //e.model_transform = glm::mat4(1.0f);
        e.bounding_sphere.radius = 1.0f;
        e.bounding_sphere.center = glm::vec3(0.0f);
        return e;
    }

    void draw(Shader shader) {
        shader.use();

        // Assumes having properly recalculated the model matrix
        shader.setModelMatrix(model_matrix);

        // Boolean check much cheaper than setting
        // Allows us to not have to store the handle to uSelected explicitly
        // since the actual set is exceedingly rare
        if (is_selected) {
            shader.setBool("uSelected", true);
            model->draw(shader);
            shader.setBool("uSelected", false);
        }
        else {
            model->draw(shader);
        }

        // Also assume that each following call to draw will properly calculate
        // and set their own model matrix (important to keep in mind)
        // Saves a lot of cycles, but might bite us in the butt at some point
    }

    float ray_test(glm::vec3 p, glm::vec3 dir) {
        // TODO: account for transformations
        //p = glm::inverse(model_matrix)
        return bounding_sphere.ray_test(p, dir);
    }

    // Saves a LOT of time if most entities are static
    void recalculate_matrix() {
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, position);
        model_matrix = glm::scale(model_matrix, scale);
        // TODO: Quaternions
        model_matrix = glm::rotate(model_matrix, rotation.z, glm::vec3(0, 0, 1));
        model_matrix = glm::rotate(model_matrix, rotation.y, glm::vec3(0, 1, 0));
        model_matrix = glm::rotate(model_matrix, rotation.x, glm::vec3(1, 0, 0));
        model_matrix = model_matrix * model->transform;
    }

    void setPRS(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl) {
        position = pos;
        bounding_sphere.center = pos;
        rotation = rot;
        scale = scl;
        recalculate_matrix();
    }

    glm::vec3 getPosition() {
        return position;
    }

    void translate(glm::vec3 delta) {
        position += delta;
        bounding_sphere.center = position;
        recalculate_matrix();
    }

    void setPosition(glm::vec3 pos) {
        bounding_sphere.center = pos;
        position = pos;
        recalculate_matrix();
    }

    glm::vec3 getRotation() {
        return rotation;
    }

    void setRotation(glm::vec3 rot) {
        rotation = rot;
        recalculate_matrix();
    }

    glm::vec3 getScale() {
        return scale;
    }

    void setScale(glm::vec3 scl) {
        scale = scl;
        recalculate_matrix();
    }
};
#endif
