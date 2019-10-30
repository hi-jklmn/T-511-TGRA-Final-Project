#include <vector>
#include <glm/glm.hpp>

#ifndef ANIMATION_H
#define ANIMATION_H

template <class T>
struct Bezier {
  vector<T> control_points;

  T at(double t) {
    t = t - (long)t;//glm::clamp(t, 0.0, 1.0);

    if (control_points.size() == 1)
      return control_points[0];

    Bezier b;

    for (int i = 0; i < control_points.size() - 1; i++) {
      b.control_points.push_back(glm::mix(control_points[i], control_points[i+1], t));
    }

    return b.at(t);
  }
};

template <class T>
struct Animation {
  bool loop, active;
  double scale, time;

  T last;
  int current_path;
  vector<Bezier<T>> paths;

  static Animation Init() {
    Animation a;
    a.current_path = 0;
    a.scale        = 1.0;
    a.time         = 0.0;
    a.loop         = false;
    a.active       = true;
    a.local        = true;
    return a;
  }

  T update(T pos, double deltaTime) {
    if (time == 0.0) {
      last = pos;
    }
    time = glm::clamp(time + deltaTime / scale, 0.0, 1.0);
    return last;
  }

};

#endif
