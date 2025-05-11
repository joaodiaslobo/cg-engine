#include "Light.hpp"

#include <GL/glew.h>

void Light::setPosition(const glm::vec3& pos) { position = pos; }

void Light::setColor(const glm::vec3& col) { color = col; }

void Light::setCutoff(float cutof) { cutoff = cutof; }

glm::vec3& Light::getPosition() { return position; }

glm::vec3& Light::getColor() { return color; }

float Light::getCutoff() { return cutoff; }

LightType Light::getType() { return type; }

void Light::setType(LightType t) { type = t; }

void Light::setDirection(const glm::vec3& dir) { direction = dir; }

glm::vec3& Light::getDirection() { return direction; }

void Light::render(int lightIndex) {
  glm::vec4 dir = glm::vec4(direction, 0.0f);
  glm::vec4 pos = glm::vec4(position, 1.0f);

  glEnable(GL_LIGHT0 + lightIndex);
  glLightfv(GL_LIGHT0 + lightIndex, GL_DIFFUSE, &color.x);
  glLightfv(GL_LIGHT0 + lightIndex, GL_SPECULAR, &color.x);

  switch (type) {
    case DIRECTIONAL:
      glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, &dir.x);
      break;
    case POINT:
      glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, &pos.x);
      break;
    case SPOTLIGHT:
      glLightfv(GL_LIGHT0 + lightIndex, GL_POSITION, &pos.x);
      glLightfv(GL_LIGHT0 + lightIndex, GL_SPOT_DIRECTION, &dir.x);
      glLightf(GL_LIGHT0 + lightIndex, GL_SPOT_CUTOFF, cutoff);
      break;
    default:
      break;
  }
}