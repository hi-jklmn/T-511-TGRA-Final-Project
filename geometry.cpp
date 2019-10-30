#include "geometry.h"
#include <cstdio>

float Point::ray_test(glm::vec3 p, glm::vec3 d) {	
	glm::vec3 to_center = glm::vec3(center - p);
	// If d is collinear with the vector from p
	// to our center, then there is a solution
	// to position = (p + t*d)
	// t = (position - p) / d
	float epsilon = 0.0000001f;
	float x = (center.x - p.x) / d.x;
	float y = (center.y - p.y) / d.y;
	float z = (center.z - p.z) / d.z;

	if (glm::abs(x - y) < epsilon && glm::abs(y-z) < epsilon) {
		return glm::length(to_center) / glm::length(d);
	}

	return INFINITY;
}

float Sphere::ray_test(glm::vec3 p, glm::vec3 d) {
	// Want to find:
	// ||(p+dt) - center|| - radius = 0
	// (p+dt)^2 - 2*(p+dt)center + center^2
	// Then quadratic formula comes in and saves us
	// t = -b +/- sqrt(b^2 - c)
	float b, c, test;

	b = glm::dot(glm::normalize(d), center - p);
	c = glm::dot(center - p, center-p) - radius*radius;

	test = b*b - c;

	if (test >= 0) {
		float t1, t2, sqrt_test;
		sqrt_test = glm::sqrt(test);
		t1 = -b + sqrt_test;
		t2 = -b - sqrt_test;

		glm::vec3 res1 = p + t1 * d;
		glm::vec3 res2 = p + t2 * d;

		if (test == 0)
			return t1;
		return t1 < t2 ? t1 : t2;
	}

	return INFINITY;
}

float Plane::ray_test(glm::vec3 p, glm::vec3 d) {
  // Want to find:
  // dot(normal, dt+p) - dot(normal, center) = 0
  return dot(center - p, normal) / dot(d, normal);
}
