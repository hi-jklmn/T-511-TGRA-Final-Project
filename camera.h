#ifndef CAMERA_H
#define CAMERA_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

class Camera {
public:
    static constexpr float DEFAULT_YAW         = -90.0f;
    static constexpr float DEFAULT_PITCH       =  0.0f;
    static constexpr float DEFAULT_SPEED       =  2.5f;
    static constexpr float DEFAULT_SENSITIVITY =  10.0f;
    static constexpr float DEFAULT_ZOOM        =  70.0f;
    static constexpr float ASPECT_RATIO        =  1024.0f / 768.0f; //TODO

    enum Movement {
        FORWARD,
        BACKWARD,
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 world_up;

    float yaw;
    float pitch;

    float movement_speed;
    float mouse_sensitivity;

    float zoom;
    float aspect_ratio;

    static Camera Default() {
        Camera c;
        c.position          = glm::vec3(0.0f);
        c.world_up          = glm::vec3(0.0f, 1.0f, 0.0f);
        c.front             = glm::vec3(0.0f, 0.0f, -1.0f);
        c.yaw               = Camera::DEFAULT_YAW;
        c.pitch             = Camera::DEFAULT_PITCH;
        c.zoom              = Camera::DEFAULT_ZOOM;
        c.movement_speed    = Camera::DEFAULT_SPEED;
        c.mouse_sensitivity = Camera::DEFAULT_SENSITIVITY;
        c.aspect_ratio      = ASPECT_RATIO;
        c.updateCameraVectors();
        return c;
    }

    static Camera FromVectors(
        glm::vec3 position,
        glm::vec3 up,
        float yaw,
        float pitch
    ) {
        Camera c = Camera::Default();
        c.position = position;
        c.world_up = up;
        c.yaw      = yaw;
        c.pitch    = pitch;
        c.updateCameraVectors();
        return c;
    }

    static Camera FromScalars(
        float posX, float posY, float posZ,
        float upX,  float upY,  float upZ,
        float yaw,  float pitch
    ) {
        Camera c;
        c.position = glm::vec3(posX, posY, posZ);
        c.world_up = glm::vec3(upX,  upY,  upZ);
        c.yaw      = yaw;
        c.pitch    = pitch;
        c.movement_speed    = Camera::DEFAULT_SPEED;
        c.mouse_sensitivity = Camera::DEFAULT_SENSITIVITY;
        c.updateCameraVectors();
        return c;
    }

    glm::mat4 getViewMatrix() {
        return glm::lookAt(position, position + front, up);
    }

    glm::mat4 getProjectionMatrix() {
        return glm::perspective(glm::radians(zoom), aspect_ratio, 0.1f, 100.0f);
    }

    void setPosition(glm::vec3 pos) {
        position = pos;
    }

    void translate(Camera::Movement direction, float deltaTime) {

        float velocity = movement_speed * deltaTime;

        if (direction == FORWARD)
            position += front * velocity;
        if (direction == BACKWARD)
            position -= front * velocity;
        if (direction == LEFT)
            position -= right * velocity;
        if (direction == RIGHT)
            position += right * velocity;
        if (direction == UP)
            position += up * velocity;
        if (direction == DOWN)
            position -= up * velocity;
    }

    void setRotation(float theta, float phi) {
        yaw   = theta;
        pitch = phi;
        updateCameraVectors();
    }

    // Anyone need fraternity name ideas?
    void rotate(float delta_theta, float delta_phi) {
        delta_theta *= mouse_sensitivity;
        delta_phi   *= mouse_sensitivity;

        yaw   += delta_theta;
        pitch += delta_phi;

        updateCameraVectors();
    }

private:
    void updateCameraVectors()
    {
        glm::vec3 new_front;
        new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        new_front.y = sin(glm::radians(pitch));
        new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        front = glm::normalize(new_front);
        right = glm::normalize(glm::cross(front, world_up));
        up    = glm::normalize(glm::cross(right, front));
    }
};
#endif
