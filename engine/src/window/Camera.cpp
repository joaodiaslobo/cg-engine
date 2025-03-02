#include "Camera.hpp"

void Camera::render() {
  gluLookAt(position.x, position.y, position.z, lookingAt.x, lookingAt.y,
            lookingAt.z, up.x, up.y, up.z);
}

void Camera::setPosition(glm::vec3 position) { this->position = position; }

void Camera::setLookingAt(glm::vec3 lookingAt) { this->lookingAt = lookingAt; }

void Camera::setFov(float fov) { this->fov = fov; }

void Camera::setNear(float near) { this->near = near; }

void Camera::setFar(float far) { this->far = far; }

void Camera::setUp(glm::vec3 up) { this->up = up; }

void Camera::setMode(CameraMode newMode) {
  mode = newMode;

  GLFWwindow* window = glfwGetCurrentContext();
  if (mode == CameraMode::FREECAM) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  } else {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

void Camera::update(float deltaTime) {
  if (mode == CameraMode::TURNTABLE) {
    turntableAngle += deltaTime * 30.0f;
    float radians = glm::radians(turntableAngle);
    position.x = lookingAt.x + turntableRadius * cos(radians);
    position.z = lookingAt.z + turntableRadius * sin(radians);
  } else if (mode == CameraMode::FREECAM) {
    vec3 forward = glm::normalize(lookingAt - position);
    vec3 right = glm::normalize(glm::cross(forward, up));

    if (keys[GLFW_KEY_W]) position += forward * moveSpeed * deltaTime;
    if (keys[GLFW_KEY_S]) position -= forward * moveSpeed * deltaTime;
    if (keys[GLFW_KEY_A]) position -= right * moveSpeed * deltaTime;
    if (keys[GLFW_KEY_D]) position += right * moveSpeed * deltaTime;
    if (keys[GLFW_KEY_SPACE]) position += up * moveSpeed * deltaTime;
    if (keys[GLFW_KEY_LEFT_SHIFT]) position -= up * moveSpeed * deltaTime;

    lookingAt = position + forward;
  }
}

void Camera::processMouseMovement(double xpos, double ypos) {
  if (mode != CameraMode::FREECAM) return;

  if (firstMouse) {
    lastX = xpos;
    lastY = ypos;
    firstMouse = false;
  }

  float xOffset = static_cast<float>(xpos - lastX) * sensitivity;
  float yOffset = static_cast<float>(lastY - ypos) * sensitivity;
  lastX = xpos;
  lastY = ypos;

  yaw += xOffset;
  pitch += yOffset;

  if (pitch > 89.0f) pitch = 89.0f;
  if (pitch < -89.0f) pitch = -89.0f;

  updateView();
}

void Camera::processKeyboard(int key, int action) {
  if (action == GLFW_PRESS) keys[key] = true;
  if (action == GLFW_RELEASE) keys[key] = false;
}

void Camera::updateView() {
  vec3 front;
  front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
  front.y = sin(glm::radians(pitch));
  front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

  lookingAt = position + glm::normalize(front);
}