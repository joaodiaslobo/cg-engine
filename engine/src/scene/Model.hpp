#pragma once

#include <glm/glm.hpp>
#include <optional>
#include <string>
#include <vector>

using glm::vec2;
using glm::vec3;
using glm::vec4;
using std::optional;
using std::string;
using std::vector;

struct Material {
  vec3 ambient = vec4(200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f);
  vec3 diffuse = vec4(50.0f / 255.0f, 50.0f / 255.0f, 50.0f / 255.0f, 1.0f);
  vec3 specular = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  vec3 emission = vec4(0.0f, 0.0f, 0.0f, 1.0f);
  float shininess = 0.0f;
};

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
  Material material;

 public:
  Model() : hasTexture(false), hasNormals(false), textureID(0){};
  void addVertex(vec3 vertex);
  void addNormal(vec3 normal);
  void addTexCoord(vec2 texCoord);
  void addIndex(uint32_t index);
  void setName(const string& name) { this->name = name; }
  void render();
  void renderNormals(float scale);
  void setMaterial(const Material& mat) { material = mat; }
  Material getMaterial() const { return material; }
  const string& getName() const { return name; }
  void sendModelToGPU();
  bool hasTextureMapping() const { return hasTexture; }
  bool hasNormalMapping() const { return hasNormals; }
  size_t getVertexCount() const { return vertices.size(); }
};

optional<Model> loadModel(const string& filename);