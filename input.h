#ifndef INPUT_H
#define INPUT_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

// Eh, might become instantiate-able
class Input {

public:
  static glm::vec4 getViewPortDimensions() {
      struct {
        int minX, minY, maxX, maxY;
      } vp;
      glGetIntegerv(GL_VIEWPORT, (int*)&vp);
      return glm::vec4(vp.minX, vp.minY, vp.maxX, vp.maxY);
  }

  static glm::vec2 getCursorScreenPosition(GLFWwindow* window) {
      glm::vec4 vp = Input::getViewPortDimensions();
      double mx, my;
      glfwGetCursorPos(window, &mx, &my);
      return glm::vec2(mx, vp.w - my);
  }

  static glm::vec3 getCursorWorldPosition(GLFWwindow* window, Camera camera) {
      glm::vec3 cursor_pos = glm::unProject(
          glm::vec3(Input::getCursorScreenPosition(window), 0.0),
          camera.getViewMatrix(),
          camera.getProjectionMatrix(),
          Input::getViewPortDimensions()
      );

      return cursor_pos;
  }
};

#endif
