#include "Light.hpp"

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