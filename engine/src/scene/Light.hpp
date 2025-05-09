#pragma once
#include <glm/glm.hpp>

enum LightType { DIRECTIONAL, POINT, SPOTLIGHT };

class Light {
 private:
  glm::vec3 position;
  glm::vec3 direction;
  glm::vec3 color;
  float cutoff;
  LightType type;

 public:
  void setPosition(const glm::vec3& pos);
  void setColor(const glm::vec3& col);
  void setCutoff(float cutoff);
  void setType(LightType t);
  void setDirection(const glm::vec3& dir);
  void render(int lightIndex);

  glm::vec3& getDirection();
  glm::vec3& getPosition();
  glm::vec3& getColor();
  float getCutoff();
  LightType getType();
};