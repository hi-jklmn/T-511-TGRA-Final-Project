#ifndef PLAYER_H
#define PLAYER_H

#include <glm/glm.hpp>

#include "camera.h"
#include "shader.h"

class Player {
 public:
  Entity entity;
  Camera camera;
  float speed;
  float camera_theta;
  float camera_phi;
  glm::vec3 target_rotation;

  static Player FromEntity(Entity e) {
    Player p;
    p.entity = e;
    p.camera = Camera::Default();
    p.centerCamera();
    p.camera_theta = 45.0f;
    p.camera_phi = 45.0f;
    p.target_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    p.speed = 2.0f;
    return p;
  }

  void draw(Shader shader) {
    entity.draw(shader);
  }

  void processInput(GLFWwindow* window, float deltaTime) {

    entity.setRotation(glm::mix(entity.getRotation(), target_rotation, 0.1f));
    target_rotation = glm::vec3(0.0, glm::radians(180 + camera_theta), 0.0);

    glm::vec3 playerDelta = glm::vec3(0.0f);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
      playerDelta += camera.front;
    }
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
      playerDelta -= camera.right;
    }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
      playerDelta -= camera.front;
    }
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
      playerDelta += camera.right;
    }

    playerDelta.y = 0.0f;

    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS){
      camera_theta += 90.0f * deltaTime;
    }
    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
      camera_phi = glm::clamp(camera_phi + 40.0f * deltaTime, 30.0f, 60.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
      camera_phi = glm::clamp(camera_phi - 40.0f * deltaTime, 30.0f, 60.0f);
    }
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
      camera_theta -= 90.0f * deltaTime;
    }

    if(glm::length(playerDelta) > 0) {
      entity.translate(normalize(playerDelta) * speed * (float)deltaTime);
    }

    centerCamera();
  }

 private:
  void centerCamera(){
    float radius = 2.0f;
    float theta = glm::radians(camera_theta);
    float phi   = glm::radians(camera_phi);
    glm::vec3 sphere_coordinate = glm::vec3(
                              glm::sin(theta) * glm::sin(phi),
                              glm::cos(phi),
                              glm::cos(theta) * glm::sin(phi)

    );
    glm::vec3 cam_pos = entity.getPosition() + radius * sphere_coordinate;
    camera.setPosition(cam_pos);
    camera.setRotation(-90-camera_theta, camera_phi - 80);
  }

};

#endif
