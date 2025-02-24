#include "Camera.hpp"

void Camera::render() {
  gluLookAt(position.x, position.y, position.z, lookingAt.x, lookingAt.y,
            lookingAt.z, up.x, up.y, up.z);
}

void Camera::setPosition(glm::vec3 position) { this->position = position; }

void Camera::setLookingAt(glm::vec3 lookingAt) { this->lookingAt = lookingAt; }

void Camera::setUp(glm::vec3 up) { this->up = up; }