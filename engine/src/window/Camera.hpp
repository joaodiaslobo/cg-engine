#pragma once

#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <vector>

using glm::vec3;

enum class CameraMode { STATIC, TURNTABLE, FREECAM };

class Camera {
 private:
  float fov = 60.0f;
  float near = 1.0f;
  float far = 1000.0f;
  vec3 position = vec3(0.0f, 0.0f, 0.0f);
  vec3 lookingAt = vec3(0.0f, 0.0f, 0.0f);
  vec3 up = vec3(0.0f, 1.0f, 0.0f);
  CameraMode mode = CameraMode::STATIC;
  float turntableAngle = 0.0f;
  float turntableRadius = 5.0f;
  float moveSpeed = 5.0f;
  float turnSpeed = 90.0f;
  float yaw = -90.0f;
  float pitch = 0.0f;
  float sensitivity = 0.1f;
  bool firstMouse = true;
  double lastX, lastY;
  bool keys[1024] = {false};

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
  void setMode(CameraMode newMode);
  void render();
  CameraMode getMode() { return mode; }
  float getFov() { return fov; }
  float getNear() { return near; }
  float getFar() { return far; }
  vec3 getPosition() { return position; }
  vec3 getLookingAt() { return lookingAt; }
  vec3 getUp() { return up; }
  void processKeyboard(int key, int action);
  void processMouseMovement(double xpos, double ypos);
  void update(float deltaTime);
  void updateView();
};