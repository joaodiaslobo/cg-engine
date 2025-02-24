#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

using glm::vec3;

class Camera {
 private:
  float fov = 60.0f;
  float near = 1.0f;
  float far = 1000.0f;
  vec3 position = vec3(0.0f, 0.0f, 0.0f);
  vec3 lookingAt = vec3(0.0f, 0.0f, 0.0f);
  vec3 up = vec3(0.0f, 1.0f, 0.0f);

 public:
  Camera(float fov = 60.0f, float near = 1.0f, float far = 1000.0f,
         vec3 position = vec3(0.0f, 0.0f, 0.0f),
         vec3 lookingAt = vec3(0.0f, 0.0f, 0.0f),
         vec3 up = vec3(0.0f, 1.0f, 0.0f))
      : fov(fov),
        near(near),
        far(far),
        position(position),
        lookingAt(lookingAt),
        up(up) {}
  void setPosition(vec3 position);
  void setLookingAt(vec3 lookingAt);
  void setUp(vec3 up);
  void render();
  float getFov() { return fov; }
  float getNear() { return near; }
  float getFar() { return far; }
  vec3 getPosition() { return position; }
  vec3 getLookingAt() { return lookingAt; }
  vec3 getUp() { return up; }
};