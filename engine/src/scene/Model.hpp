#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

using glm::vec2;
using glm::vec3;
using std::optional;
using std::string;
using std::vector;

class Model {
 private:
  string name;
  vector<vec3> vertices;
  vector<vec3> normals;
  vector<vec2> texCoords;
  vector<uint32_t> indexes;
  uint32_t vertexBuffer;
  uint32_t normalBuffer;
  uint32_t texCoordBuffer;
  uint32_t indexBuffer;
  uint32_t textureID;
  bool hasTexture;
  bool hasNormals;

 public:
  Model() : hasTexture(false), hasNormals(false), textureID(0){};
  void addVertex(vec3 vertex);
  void addNormal(vec3 normal);
  void addTexCoord(vec2 texCoord);
  void addIndex(uint32_t index);
  void setName(const string& name) { this->name = name; }
  void render();
  void renderNormals(float scale);
  const string& getName() const { return name; }
  void sendModelToGPU();
  bool hasTextureMapping() const { return hasTexture; }
  bool hasNormalMapping() const { return hasNormals; }
  size_t getVertexCount() const { return vertices.size(); }
};

optional<Model> loadModel(const string& filename);