#include <cstdlib>
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include "shader.h"
#include "models.h"
#include "camera.h"
#include "entity.h"
#include "scene.h"
#include "animation.h"

#include "debug.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#define WIDTH  1024
#define HEIGHT 768

#define DEBUG true

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern "C" void error_callback(int error, const char* description);
void panic(const char* description);
void processInput(GLFWwindow *window);
void mouse_button_callback(
	GLFWwindow* window, int button, int action, int mods
);

// Dirty, filthy global scope
float deltaTime = 0.0f;
float lastFrame = 0.0f;

Debug debug;

Shader ourShader;
Shader debugShader;
Camera camera;

Scene  scene;
Entity sphere_test;

glm::mat4 projection;
glm::mat4 view;

int main() {
  printf("Started OpenGL-Testing v%d.%d\n", MAJOR_VERSION, MINOR_VERSION);

  if (!glfwInit()) panic("Could not initialize glfw\n");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL-Testing", NULL, NULL);
  if(!window) panic("Could not create window\n");

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // Just so that glfw knows how to give us them sweet, sweet errors
  glfwSetErrorCallback(error_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

  glfwMakeContextCurrent(window);

  printf("Running OpenGL Version %s\n", glGetString(GL_VERSION));
  printf("Running GLSL Version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Thanks I hate it
  glewInit();

	debug = Debug::init();

  glEnable(GL_DEPTH_TEST);

  // Shader Setup
  ourShader = Shader::FromPath("shader.vert", "shader.frag");
  ourShader.use();
  Shader skyBoxShader = Shader::FromPath("skybox.vert", "skybox.frag");
  debugShader = Shader::FromPath("debug.vert", "debug.frag");

  camera = Camera::Default();
  camera.position = glm::vec3(0.0, 3.0, 3.0f);
  camera.Rotate(500.0f, -400.0f);
  camera.aspect_ratio = (float) WIDTH / (float) HEIGHT;

  Model nanosuit = Model::FromPath("res/nanosuit/nanosuit.obj");

	Model sphere_model = Model::FromMesh(Mesh::Sphere());

  Mesh skySphere = Mesh::Sphere();

  PointLight light = PointLight::Default();
  light.is_lit = true;
  light.position = glm::vec4(8.0f, 2.0f, 8.0f, 1.0f);
  light.color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  light.radius = 10.0f;
  scene.point_lights.push_back(light);
  scene.point_lights.push_back(light);

  DirectionalLight dlight = DirectionalLight::Default();
  dlight.direction = glm::vec4(0.8f, 2.0f, 1.2f, 0.0f);
  dlight.color     = glm::vec4(0.1f);
  dlight.ambient   = glm::vec4(0.0f);
  scene.directional_lights.push_back(dlight);

  int map[10][10] = {
      {1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,0,0,1,0,0,1},
      {1,0,0,1,1,1,1,0,0,1},
      {1,0,0,1,0,0,1,0,0,1},
      {1,0,0,0,0,0,1,0,0,1},
      {1,0,0,1,1,1,1,1,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,1,1,0,0,0,0,0,0,1},
      {1,0,0,0,0,0,0,0,0,1},
      {1,1,1,1,1,1,1,1,1,1}
  };

  Texture tile_diffuse_texture = Texture::FromPath("res/brickwall/brickwall.jpg");
  tile_diffuse_texture.type = "uTextureDiffuse";

  Texture tile_normal_texture = Texture::FromPath("res/brickwall/brickwall_normal.jpg");
  tile_normal_texture.type = "uTextureNormal";

  Mesh tile_mesh  = Mesh::Cube();
  tile_mesh.material = Material::Default();
  tile_mesh.textures.push_back(tile_diffuse_texture);
  tile_mesh.textures.push_back(tile_normal_texture);

  Model tile_floor = Model::FromMesh(tile_mesh);
  Model tile_wall  = Model::FromMesh(tile_mesh);

  float map_scale = 1.0f;


  for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 10; j++) {
      Entity* e = new Entity;
      glm::vec3 pos = glm::vec3(0.0f);

      if (map[i][j] == 0) {
        *e = Entity::FromModel(&tile_floor);
        pos.y = 0.0f + 0.01f * (rand() % 3);
      } else {
        *e = Entity::FromModel(&tile_wall);
        pos.y = 1.0f;
      }

      pos.x = i;
      pos.z = j;
      e->setPosition(pos);
      e->setScale(glm::vec3(map_scale));

      scene.entities.push_back(e);
    }
  }

  Entity * suit = new Entity;
  *suit = Entity::FromModel(&nanosuit);
  suit->setPosition(glm::vec3(2.0f, 0.0f, 2.0f));
  suit->setScale(glm::vec3(0.2f));
  scene.entities.push_back(suit);

  Bezier<glm::vec4> b1;
  b1.control_points.push_back(glm::vec4(2.0f, 1.0f, 2.0f, 1.0f));
  b1.control_points.push_back(glm::vec4(9.0f, 1.0f, 2.0f, 1.0f));
  b1.control_points.push_back(glm::vec4(9.0f, 1.0f, 9.0f, 1.0f));
  b1.control_points.push_back(glm::vec4(2.0f, 1.0f, 9.0f, 1.0f));
  b1.control_points.push_back(glm::vec4(2.0f, 1.0f, 2.0f, 1.0f));

  Bezier<glm::vec4> b2;
  b2.control_points.push_back(glm::vec4(8.0f, 1.5f, 2.0f, 1.0f));
  b2.control_points.push_back(glm::vec4(2.0f, 1.5f, 2.0f, 1.0f));
  b2.control_points.push_back(glm::vec4(2.0f, 1.5f, 8.0f, 1.0f));
  b2.control_points.push_back(glm::vec4(8.0f, 1.5f, 8.0f, 1.0f));
  b2.control_points.push_back(glm::vec4(2.0f, 1.5f, 2.0f, 1.0f));
  b2.control_points.push_back(glm::vec4(8.0f, 1.5f, 2.0f, 1.0f));

  Bezier<float> b3;
  b3.control_points.push_back(1.0f);
  b3.control_points.push_back(0.0f);
  b3.control_points.push_back(0.0f);
  b3.control_points.push_back(1.0f);

  // Viewport blah
  glViewport(0, 0, WIDTH, HEIGHT);

  double px = 0.0, py = 0.0;

  while (!glfwWindowShouldClose(window)) {
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    glEnable(GL_FRAMEBUFFER_SRGB);
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();
    ourShader.setCamera(camera);

    if (scene.selected_entity && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
      int vp[4];

      glGetIntegerv(GL_VIEWPORT, vp);

      glm::vec3 cursor_pos = glm::unProject(
          glm::vec3((float)mx, (float)(HEIGHT - my), 0),
          camera.getViewMatrix(),
          camera.getProjectionMatrix(),
          glm::vec4(vp[0], vp[1], vp[2], vp[3])
      );

      glm::vec3 dir = glm::normalize(camera.position - cursor_pos);

      float t = Plane::PointNormal(scene.selected_entity->getPosition(), camera.front).ray_test(cursor_pos, dir);

      scene.selected_entity->setPosition(cursor_pos + t * dir);
    }

    scene.point_lights[0].position = b1.at(glfwGetTime() / 5.0);
    scene.point_lights[1].position = b2.at(glfwGetTime() / 5.0);

    float at1 = b3.at(glfwGetTime() / 5.0);
    float at2 = b3.at(glfwGetTime() / 5.0 + 0.66);
    float at3 = b3.at(glfwGetTime() / 5.0 +0.33);

    scene.point_lights[0].color = glm::vec4(at1, at2, at3, 1.0f);
    scene.point_lights[1].color = glm::vec4(at2, at3, at1, 1.0f);

    scene.draw(ourShader);

    if (DEBUG) {
      debugShader.use();
      debugShader.setCamera(camera);
      for (PointLight p : scene.point_lights) {
        debug.draw_point_light(debugShader, p);
      }
      debug.draw_grid(debugShader);
    }

    glDepthFunc(GL_LEQUAL);

    skyBoxShader.use();
    skyBoxShader.setCamera(camera);
    skySphere.draw(skyBoxShader);

    glDepthFunc(GL_LESS);

    px = mx;
    py = my;
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Cleanup
  glfwDestroyWindow(window);
  glfwTerminate();

  printf("Exited correctly\n");

  return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

extern "C" void error_callback(int error, const char* description) {
  fprintf(stderr, "Error: %s\n", description);
}

void panic(const char* description) {
  error_callback(1, description);
  std::exit(1);
}

void mouse_button_callback(
	GLFWwindow* window, int button, int action, int mods
) {
	switch(button){
		case GLFW_MOUSE_BUTTON_LEFT:
			if(action == GLFW_PRESS) {
				int vp[4];
				glGetIntegerv(GL_VIEWPORT, vp);
				double mx, my;
				glfwGetCursorPos(window, &mx, &my);
				glm::vec3 cursor_pos = glm::unProject(
								glm::vec3((float)mx, (float)(HEIGHT - my), 0),
								camera.getViewMatrix(),
								camera.getProjectionMatrix(),
								glm::vec4(vp[0], vp[1], vp[2], vp[3])
				);

				glm::vec3 dir = glm::normalize(camera.position - cursor_pos);

        scene.select_by_ray_cast(cursor_pos, dir);
			}
	}
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

	float look_speed = camera.mouse_sensitivity * deltaTime;
  // Camera controls
	if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
		camera.Rotate(-look_speed, 0.0f);
	if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		camera.Rotate(+look_speed, 0.0f);
	if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		camera.Rotate(0.0f, -look_speed);
	if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		camera.Rotate(0.0f, +look_speed);

	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.Translate(Camera::Movement::FORWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.Translate(Camera::Movement::LEFT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.Translate(Camera::Movement::BACKWARD, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.Translate(Camera::Movement::RIGHT, deltaTime);

	if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		camera.Translate(Camera::Movement::LEFT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		camera.Translate(Camera::Movement::RIGHT, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		camera.Translate(Camera::Movement::UP, deltaTime);
	if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		camera.Translate(Camera::Movement::DOWN, deltaTime);
}
