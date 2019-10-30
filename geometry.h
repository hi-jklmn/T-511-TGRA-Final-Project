#include <glm/glm.hpp>

#ifndef GEOMETRY_H
#define GEOMETRY_H
struct Geometry {
	glm::vec3 center;
	virtual float ray_test(glm::vec3 p, glm::vec3 d) = 0;
};

struct Point : public Geometry {
	float ray_test(glm::vec3 p, glm::vec3 d);
};

//struct Line : public Geometry {
//	glm::vec3 direction;
//}
//
struct Sphere : public Geometry{
	float radius;
	float ray_test(glm::vec3 p, glm::vec3 d);
};

struct Plane : public Geometry {
	glm::vec3 normal;

  static Plane PointNormal(glm::vec3 p, glm::vec3 n) {
    Plane ret;
    ret.center = p;
    ret.normal = n;
    return ret;
  }

	float ray_test(glm::vec3 p, glm::vec3 d);
};
//
//struct Pill : public Geometry {
//}
#endif
