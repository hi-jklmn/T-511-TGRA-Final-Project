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
        c.updateCameraVectors();
        c.aspect_ratio = 1.0f;
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

    void Translate(Camera::Movement direction, float deltaTime) {

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

    void Rotate(
				float xoffset,
				float yoffset,
				GLboolean constrainPitch = true
		) {
        xoffset *= mouse_sensitivity;
        yoffset *= mouse_sensitivity;

        yaw   += xoffset;
        pitch += yoffset;

        if (constrainPitch)
        {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        updateCameraVectors();
    }

    void ProcessMouseScroll(float yoffset)
    {
        if (zoom >= 1.0f && zoom <= 45.0f)
            zoom -= yoffset;
        if (zoom <= 1.0f)
            zoom = 1.0f;
        if (zoom >= 45.0f)
            zoom = 45.0f;
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
