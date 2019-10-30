#include <vector>

#include "entity.h"
#include "shader.h"

#ifndef SCENE_H
#define SCENE_H

class Scene {
public:
  ~Scene() { for (Entity* e : entities) { delete e; } }

  Entity* selected_entity;

  vector<Entity*>            entities;
  vector<PointLight>         point_lights;
  vector<DirectionalLight>   directional_lights;

  void update() {

  }

  void draw(Shader shader) {
      for (int i = 0; i < directional_lights.size(); i++) {
        shader.setDirectionalLight(i, directional_lights[i]);
      }

      for (int i = 0; i < point_lights.size(); i++) {
          shader.setPointLight(i, point_lights[i]);
      }

      for (Entity* e : entities) {
          e->draw(shader);
      }
  }

  bool select_by_ray_cast(glm::vec3 p, glm::vec3 dir) {
      float min_t = INFINITY;

      // Hurts my soul, looking forward to segfault
      selected_entity = NULL;

      for (int i = 0; i < entities.size(); i++) {
          entities[i]->is_selected = false;
          float t = entities[i]->bounding_sphere.ray_test(p, dir);
          if (t < min_t) {
            selected_entity = entities[i];
          }
      }

      if (selected_entity) {
          selected_entity->is_selected = true;
          return true;
      }

      return false;
  }

};

#endif
