#include "Model.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Settings.hpp"
#include "debug/Logger.hpp"

using std::optional;

static debug::Logger logger;

/**
 * @brief Parses the an index from a given string_view.
 *
 * This function extracts an integer from a string_view that may contain
 * '/' characters. If the '/' character is found, the function returns the
 * substring at the specified index position (0 for vertex, 1 for texcoord, 2
 * for normal). If there's no value at the specified index (empty string),
 * return 0.
 *
 * @param string The input string_view containing the index.
 * @param index The index of the component to extract (0=vertex, 1=texcoord,
 * 2=normal).
 * @return The parsed integer value from the input string.
 */
size_t parseIndex(std::string_view string, int index) {
  size_t start = 0;
  size_t end = string.find('/');

  for (int i = 0; i < index; i++) {
    if (end == std::string::npos) {
      return 0;  // Not enough components
    }
    start = end + 1;
    end = string.find('/', start);
  }

  if (start >= string.length()) {
    return 0;  // Not enough components
  }

  if (end == std::string::npos) {
    end = string.length();
  }

  std::string component = std::string(string.substr(start, end - start));
  if (component.empty()) {
    return 0;  // Empty component
  }

  return std::stoi(component);
}

/**
 * @brief Loads a model from a file.
 *
 * This function reads a file specified by the filename and constructs a
 * Model object from the vertex, normal, texture coordinate, and face data.
 *
 * @param filename The path to the file to be loaded.
 * @return An optional Model object containing the loaded model data. If the
 * file could not be opened, an empty optional is returned.
 */
optional<Model> loadModel(const string& filename) {
  Model model;

  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  vector<vec3> vertices;
  vector<vec3> normals;
  vector<vec2> texCoords;
  bool hasTexCoords = false;
  bool hasNormals = false;

  while (std::getline(file, line)) {
    if (line[0] == '#') {
      continue;
    }

    std::istringstream stream(line);
    std::string type;
    stream >> type;

    if (type == "v") {
      vec3 vertex;
      stream >> vertex.x >> vertex.y >> vertex.z;
      vertices.push_back(vertex);
    } else if (type == "vn") {
      vec3 normal;
      stream >> normal.x >> normal.y >> normal.z;
      normals.push_back(normal);
      hasNormals = true;
    } else if (type == "vt") {
      vec2 texCoord;
      stream >> texCoord.x >> texCoord.y;
      texCoords.push_back(texCoord);
      hasTexCoords = true;
    } else if (type == "f") {
      std::string v1, v2, v3;
      stream >> v1 >> v2 >> v3;

      // Vertex indices (adjust to 0-based indexing)
      size_t vi1 = parseIndex(v1, 0) - 1;
      size_t vi2 = parseIndex(v2, 0) - 1;
      size_t vi3 = parseIndex(v3, 0) - 1;

      model.addVertex(vertices[vi1]);
      model.addVertex(vertices[vi2]);
      model.addVertex(vertices[vi3]);

      // Texture coordinate indices
      if (hasTexCoords) {
        size_t ti1 = parseIndex(v1, 1) - 1;
        size_t ti2 = parseIndex(v2, 1) - 1;
        size_t ti3 = parseIndex(v3, 1) - 1;

        if (ti1 < texCoords.size())
          model.addTexCoord(texCoords[ti1]);
        else
          model.addTexCoord(vec2(0.0f, 0.0f));

        if (ti2 < texCoords.size())
          model.addTexCoord(texCoords[ti2]);
        else
          model.addTexCoord(vec2(0.0f, 0.0f));

        if (ti3 < texCoords.size())
          model.addTexCoord(texCoords[ti3]);
        else
          model.addTexCoord(vec2(0.0f, 0.0f));
      }

      // Normal indices
      if (hasNormals) {
        size_t ni1 = parseIndex(v1, 2) - 1;
        size_t ni2 = parseIndex(v2, 2) - 1;
        size_t ni3 = parseIndex(v3, 2) - 1;

        if (ni1 < normals.size())
          model.addNormal(normals[ni1]);
        else
          model.addNormal(vec3(0.0f, 1.0f, 0.0f));

        if (ni2 < normals.size())
          model.addNormal(normals[ni2]);
        else
          model.addNormal(vec3(0.0f, 1.0f, 0.0f));

        if (ni3 < normals.size())
          model.addNormal(normals[ni3]);
        else
          model.addNormal(vec3(0.0f, 1.0f, 0.0f));
      }

      // Add indices for the triangle
      uint32_t baseIndex = model.getVertexCount() - 3;
      model.addIndex(baseIndex);
      model.addIndex(baseIndex + 1);
      model.addIndex(baseIndex + 2);
    }
  }

  model.setName(filename);
  file.close();

  model.sendModelToGPU();

  return model;
}

optional<Texture> loadTexture(const std::string& file_path) {
  stbi_set_flip_vertically_on_load(true);
  int width, height, channels;
  uint8_t* data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);

  if (!data) {
    if (stbi_failure_reason()) std::cout << stbi_failure_reason();
    return std::nullopt;
  }

  const uint32_t image_width = static_cast<uint32_t>(width);
  const uint32_t image_height = static_cast<uint32_t>(height);

  return {Texture{file_path, image_width, image_height,
                  std::unique_ptr<uint8_t>(data)}};
}

void Model::sendTextureToGPU(Texture& texture) {
  glGenTextures(1, &textureBuffer);
  glBindTexture(GL_TEXTURE_2D, textureBuffer);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture.GetWidth(),
               texture.GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE,
               texture.GetTextureData().get());
  glGenerateMipmap(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, 0);
  hasTexture = true;
}

/**
 * @brief Sends the model data to the GPU.
 *
 * This function generates OpenGL buffers for the vertex, normal, texture
 * coordinate, and index data of the model and uploads the data to the GPU.
 */
void Model::sendModelToGPU() {
  // Create and bind vertex buffer
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(),
               GL_STATIC_DRAW);

  // Create and bind normal buffer if we have normals
  if (!normals.empty()) {
    hasNormals = true;
    glGenBuffers(1, &normalBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(),
                 GL_STATIC_DRAW);
  }

  // Create and bind texture coordinate buffer if we have texture coordinates
  if (!texCoords.empty()) {
    glGenBuffers(1, &texCoordBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glBufferData(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2),
                 texCoords.data(), GL_STATIC_DRAW);
  }

  // Create and bind index buffer
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(uint32_t),
               indexes.data(), GL_STATIC_DRAW);
}

void Model::addVertex(vec3 vertex) { vertices.push_back(vertex); }

void Model::addNormal(vec3 normal) { normals.push_back(normal); }

void Model::addTexCoord(vec2 texCoord) { texCoords.push_back(texCoord); }

void Model::addIndex(uint32_t index) { indexes.push_back(index); }

/**
 * @brief Renders the normals of the model as lines.
 *
 * This function visualizes the normals of the model by drawing lines from each
 * vertex in the direction of its normal. This is useful for debugging to ensure
 * normals are loaded and calculated correctly.
 *
 * @param scale The length of the normal lines.
 */
void Model::renderNormals(float scale) {
  if (!hasNormals || normals.empty() || vertices.empty()) {
    return;
  }

  if (normals.size() != vertices.size()) {
    return;
  }

  GLboolean lighting_enabled = glIsEnabled(GL_LIGHTING);
  GLboolean texture_enabled = glIsEnabled(GL_TEXTURE_2D);

  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 0.0f, 0.0f);

  glBegin(GL_LINES);

  for (size_t i = 0; i < vertices.size(); i++) {
    const vec3& vertex = vertices[i];
    const vec3& normal = normals[i];

    glVertex3f(vertex.x, vertex.y, vertex.z);

    glVertex3f(vertex.x + normal.x * scale, vertex.y + normal.y * scale,
               vertex.z + normal.z * scale);
  }
  glEnd();

  // Reset color
  glColor3f(1.0f, 1.0f, 1.0f);

  // Restore previous OpenGL state
  if (texture_enabled) {
    glEnable(GL_TEXTURE_2D);
  }
  if (lighting_enabled) {
    glEnable(GL_LIGHTING);
  }
}

/**
 * @brief Renders the model using OpenGL.
 *
 * This function binds the vertex, normal, and texture coordinate buffers and
 * issues a draw call to render the model.
 */
void Model::render(ViewMode viewMode) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, &material.ambient.x);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, &material.diffuse.x);
  glMaterialfv(GL_FRONT, GL_SPECULAR, &material.specular.x);
  glMaterialfv(GL_FRONT, GL_EMISSION, &material.emission.x);
  glMaterialf(GL_FRONT, GL_SHININESS, material.shininess);

  logger.info("======= Rendering model: " + name + " =======");
  logger.info("Diffuse: " + std::to_string(material.diffuse.x) + ", " +
              std::to_string(material.diffuse.y) + ", " +
              std::to_string(material.diffuse.z));
  logger.info("Ambient: " + std::to_string(material.ambient.x) + ", " +
              std::to_string(material.ambient.y) + ", " +
              std::to_string(material.ambient.z));
  logger.info("Specular: " + std::to_string(material.specular.x) + ", " +
              std::to_string(material.specular.y) + ", " +
              std::to_string(material.specular.z));
  logger.info("Emission: " + std::to_string(material.emission.x) + ", " +
              std::to_string(material.emission.y) + ", " +
              std::to_string(material.emission.z));
  logger.info("Shininess: " + std::to_string(material.shininess));
  logger.info("=========================================");
  
  // Enable client states
  glEnableClientState(GL_VERTEX_ARRAY);

  // Bind vertex buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexPointer(3, GL_FLOAT, 0, 0);

  // If we have normals, bind them
  if (hasNormals) {
    glEnableClientState(GL_NORMAL_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);
    glNormalPointer(GL_FLOAT, 0, 0);
  }

  // If we have a texture, bind it
  if (hasTexture && !texCoords.empty() && viewMode != WIREFRAME) {
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glBindBuffer(GL_ARRAY_BUFFER, texCoordBuffer);
    glTexCoordPointer(2, GL_FLOAT, 0, 0);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureBuffer);
  }

  // Bind index buffer and draw
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);

  glDisableClientState(GL_VERTEX_ARRAY);

  if (hasNormals) {
    glDisableClientState(GL_NORMAL_ARRAY);
  }

  if (hasTexture && !texCoords.empty()) {
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
  }
}