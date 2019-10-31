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
#include "player.h"
#include "maze.h"

#include "input.h"
#include "debug.h"

#define MAJOR_VERSION 0
#define MINOR_VERSION 1

#define WIDTH  1024
#define HEIGHT 768

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
extern "C" void error_callback(int error, const char* description);
void panic(const char* description);
void processInput(GLFWwindow *window);
void init(); // Keep all that unchanging initialization code nice and contained
void mouse_button_callback(
	GLFWwindow* window, int button, int action, int mods
);
void mouse_callback(
  GLFWwindow* window, double xpos, double ypos
);

enum GameMode {
    DEBUG,
    PLAY
};

// Dirty, filthy global scope
// Would not normally do this, but don't have time to come up with a less awful solution
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLFWwindow* window;

Debug debug;

GameMode currentMode;

Shader ourShader;
Shader debugShader;

Camera debugCamera;

Player player;

// More chances of dereferencing null pointers.
// Programming is fun!
Camera* activeCamera;

Scene  scene;

int main() {

  init();

  debug = Debug::init();

  currentMode = GameMode::PLAY;

  // Shader Setup
  ourShader = Shader::FromPath("shaders/shader.vert", "shaders/shader.frag");
  debugShader = Shader::FromPath("shaders/debug.vert", "shaders/debug.frag");
  Shader skyBoxShader = Shader::FromPath("shaders/skybox.vert", "shaders/skybox.frag");

  debugCamera = Camera::Default();
  debugCamera.position = glm::vec3(0.0f, 3.0f, 3.0f);

  Model hand = Model::FromPath("res/hand/hand.obj");
  for(Mesh mesh : hand.meshes) {
    mesh.material = Material::DebugLight();
    mesh.textures.clear();
  }

  vector<Model> statues;
  Model statue = Model::FromPath("res/statue/12330_Statue_v1_L2.obj");
  statue.transform = glm::rotate(statue.transform, glm::radians(-90.0f), glm::vec3(1,0,0));
  statue.transform = glm::scale(statue.transform, glm::vec3(1.5f));
  statues.push_back(statue);

	Model sphere_model = Model::FromMesh(Mesh::Sphere());

  Entity player_entity = Entity::FromModel(&hand);
  player_entity.model->transform = glm::rotate(
      player_entity.model->transform,
      glm::radians(-90.0f),
      glm::vec3(1.0f, 0.0f, 0.0f)
  );
  player_entity.model->transform = glm::rotate(
      player_entity.model->transform,
      glm::radians(195.0f),
      glm::vec3(0.0f, 0.0f, 1.0f)
  );
  player = Player::FromEntity(player_entity);

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
  dlight.color     = glm::vec4(0.4f);
  dlight.ambient   = glm::vec4(0.0f);
  scene.directional_lights.push_back(dlight);

  Texture wall_diffuse_texture = Texture::FromPath("res/brickwall/brickwall.jpg");
  Texture wall_normal_texture  = Texture::FromPath("res/brickwall/brickwall_normal.jpg");
  Texture floor_diffuse_texture = Texture::FromPath("res/Brick_Wall_009/Brick_Wall_009_COLOR.jpg");
  Texture floor_normal_texture  = Texture::FromPath("res/Brick_Wall_009/Brick_Wall_009_NORM.jpg");

  // Normal maps were so not worth the effort to get them to work

  wall_diffuse_texture.type = "uTextureDiffuse";
  wall_normal_texture.type  = "uTextureNormal";

  floor_diffuse_texture.type = "uTextureDiffuse";
  floor_normal_texture.type  = "uTextureNormal";

  Mesh tile_mesh  = Mesh::Cube();

  tile_mesh.material = Material::Default();
  tile_mesh.textures.push_back(wall_normal_texture);
  Model tile_wall  = Model::FromMesh(tile_mesh);

  tile_mesh.material = Material::DebugLight();
  tile_mesh.textures.clear();
  tile_mesh.textures.push_back(floor_normal_texture);
  Model tile_floor = Model::FromMesh(tile_mesh);


  Maze maze = Maze::Default(10, 10);

  float map_scale = 1.0f;

  for (int i = 0; i < maze.tiles.size(); i++) {
    for(int j = 0; j < maze.tiles[0].size(); j++) {
      Entity* e = new Entity;
      glm::vec3 pos = glm::vec3(0.0f);
      pos.x = (i-10) * map_scale;
      pos.z = (j-10) * map_scale;

      switch(maze.tiles[i][j]) {

      case Maze::TileType::FLOOR:
        *e = Entity::FromModel(&tile_floor);
        pos.y = (-0.5f + 0.01f * (rand() % 3)) * map_scale;
        break;

      case Maze::TileType::WALL:
        *e = Entity::FromModel(&tile_wall);
        pos.y = (0.5f) * map_scale;
        break;

      case Maze::TileType::STATUE:
        *e = Entity::FromModel(&tile_floor);
        pos.y = (-0.5f + 0.01f * (rand() % 3)) * map_scale;

        Entity* s = new Entity;
        *s = Entity::FromModel(&statue);
        s->setRotation(glm::vec3(0.0f, 0.0f, glm::radians((float)(rand() % 360))));
        s->setPosition(pos + glm::vec3(0.0f, 0.50f, 0.0f) * map_scale);
        s->setScale(glm::vec3(map_scale));
        scene.entities.push_back(s);
      }

      e->setPosition(pos);
      e->setScale(glm::vec3(map_scale));

      scene.entities.push_back(e);
    }
  }


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
  glEnable(GL_FRAMEBUFFER_SRGB);

  double px = 0.0, py = 0.0;

  while (!glfwWindowShouldClose(window)) {

    // Who doesn't love pointer juggling
    if(currentMode == GameMode::PLAY) {
        activeCamera = &player.camera;
    } else if(currentMode == GameMode::DEBUG) {
        activeCamera = &debugCamera;
    }

    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    processInput(window);

    if (currentMode == GameMode::PLAY) {
      player.processInput(window, deltaTime);
    }

    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ourShader.use();
    ourShader.setCamera(*activeCamera);

    scene.point_lights[0].position = b1.at(glfwGetTime() / 5.0);
    scene.point_lights[1].position = b2.at(glfwGetTime() / 5.0);

    float at1 = b3.at(glfwGetTime() / 5.0);
    float at2 = b3.at(glfwGetTime() / 5.0 + 0.66);
    float at3 = b3.at(glfwGetTime() / 5.0 +0.33);

    scene.point_lights[0].color = glm::vec4(at1, at2, at3, 1.0f);
    scene.point_lights[1].color = glm::vec4(at2, at3, at1, 1.0f);

    scene.draw(ourShader);
    player.draw(ourShader);

    if (currentMode == GameMode::DEBUG) {
      debugShader.use();
      debugShader.setCamera(*activeCamera);

      for (PointLight p : scene.point_lights) {
        debug.draw_point_light(debugShader, p);
      }

      debug.draw_grid(debugShader);
    }

    glDepthFunc(GL_LEQUAL);

    skyBoxShader.use();
    skyBoxShader.setCamera(*activeCamera);
    skySphere.draw(skyBoxShader);

    glDepthFunc(GL_LESS);

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

void init() {
  printf("Started OpenGL-Testing v%d.%d\n", MAJOR_VERSION, MINOR_VERSION);

  if (!glfwInit()) panic("Could not initialize glfw\n");

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

  window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL-Testing", NULL, NULL);
  if(!window) panic("Could not create window\n");

  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  // Just so that glfw knows how to give us them sweet, sweet errors
  // This has saved me more time than you know
  glfwSetErrorCallback(error_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
  glfwSetCursorPosCallback(window, mouse_callback);

  glfwMakeContextCurrent(window);

  printf("Running OpenGL Version %s\n", glGetString(GL_VERSION));
  printf("Running GLSL Version %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

  // Thanks I hate it
  glewInit();

  glEnable(GL_DEPTH_TEST);

}

void mouse_callback(
  GLFWwindow* window, double xpos, double ypos
) {
  if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {

    if(scene.selected_entity) {
      glm::vec3 cursor_pos = Input::getCursorWorldPosition(window, *activeCamera);
      glm::vec3 dir = glm::normalize(activeCamera->position - cursor_pos);
      float t = Plane::PointNormal(scene.selected_entity->getPosition(), activeCamera->front)
                .ray_test(cursor_pos, dir);
      scene.selected_entity->setPosition(cursor_pos + t * dir);
    }
  }
}

void mouse_button_callback(
	GLFWwindow* window, int button, int action, int mods
) {
	switch(button){
		case GLFW_MOUSE_BUTTON_LEFT:
			if(action == GLFW_PRESS) {
        glm::vec3 cursor_pos = Input::getCursorWorldPosition(window, *activeCamera);
				glm::vec3 dir = glm::normalize(activeCamera->position - cursor_pos);
        scene.select_by_ray_cast(cursor_pos, dir);
			}
	}
}

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);

  if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    currentMode = currentMode == GameMode::DEBUG ? GameMode::PLAY : GameMode::DEBUG;

  if (currentMode == GameMode::DEBUG) {
    float look_speed = debugCamera.mouse_sensitivity * deltaTime;

    // Camera controls
    if(glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
      debugCamera.rotate(-look_speed, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
      debugCamera.rotate(+look_speed, 0.0f);
    if(glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
      debugCamera.rotate(0.0f, -look_speed);
    if(glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
      debugCamera.rotate(0.0f, +look_speed);

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::FORWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::BACKWARD, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::RIGHT, deltaTime);

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::LEFT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::RIGHT, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::UP, deltaTime);
    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
      debugCamera.translate(Camera::Movement::DOWN, deltaTime);
  }
}
