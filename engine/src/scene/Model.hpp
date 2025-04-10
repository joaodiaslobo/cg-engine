#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

using glm::vec3;
using std::optional;
using std::string;
using std::vector;

class Model {
 private:
  string name;
  vector<vec3> vertices;
  vector<uint32_t> indexes;
  uint32_t vertexBuffer;
  uint32_t indexBuffer;

 public:
  Model(){};
  void addVertex(vec3 vertex);
  void addIndex(uint32_t index);
  void setName(const string& name) { this->name = name; }
  void render();
  const string& getName() const { return name; }
  void Model::sendModelToGPU();
};

optional<Model> loadModel(const string& filename);