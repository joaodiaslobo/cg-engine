#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "engine/Settings.hpp"

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

class Texture {
 private:
  std::string texture_name;
  uint32_t image_width, image_height;
  std::unique_ptr<uint8_t> texture_data;

 public:
  Texture(const std::string &texture_name, const uint32_t image_width,
          const uint32_t image_height, std::unique_ptr<uint8_t> texture_data)
      : texture_name(texture_name),
        image_width(image_width),
        image_height(image_height),
        texture_data(std::move(texture_data)) {}
  std::string &GetName() { return texture_name; };
  uint32_t GetWidth() const { return image_width; };
  uint32_t GetHeight() const { return image_height; };
  std::unique_ptr<uint8_t> &GetTextureData() { return texture_data; };
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
  uint32_t textureBuffer;
  bool hasTexture;
  bool hasNormals;
  Material material;

 public:
  Model() : hasTexture(false), hasNormals(false){};
  void addVertex(vec3 vertex);
  void addNormal(vec3 normal);
  void addTexCoord(vec2 texCoord);
  void addIndex(uint32_t index);
  void setName(const string &name) { this->name = name; }
  void render(ViewMode viewMode);
  void renderNormals(float scale);
  void setMaterial(const Material &mat) { material = mat; }
  Material getMaterial() const { return material; }
  const string &getName() const { return name; }
  void sendTextureToGPU(Texture &texture);
  void sendModelToGPU();
  bool hasTextureMapping() const { return hasTexture; }
  bool hasNormalMapping() const { return hasNormals; }
  size_t getVertexCount() const { return vertices.size(); }
};

optional<Model> loadModel(const string &filename);
optional<Texture> loadTexture(const std::string &file_path);