#include "Model.hpp"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "debug/Logger.hpp"

using std::optional;

static debug::Logger logger;

/**
 * @brief Parses the first index from a given string_view.
 *
 * This function extracts the first integer from a string_view that may contain
 * a '/' character. If the '/' character is found, the function converts the
 * substring before the '/' to an integer. If the '/' character is not found,
 * the entire string is converted to an integer.
 * This effectively removes the '/' and any subsequent characters from the
 * string. (useful for obj files)
 *
 * @param string The input string_view containing the index.
 * @return The parsed integer value from the input string.
 */
size_t parseFirstIndex(std::string_view string) {
  if (const size_t pos = string.find('/'); pos != std::string::npos) {
    return std::stoi(std::string(string.substr(0, pos)));
  }
  return std::stoi(std::string(string));
}

/**
 * @brief Loads a model from a file.
 *
 * This function reads a file specified by the filename and constructs a
 * Model object from the vertex and face data contained in the file. The
 * function returns an optional Model object, which will be empty if the file
 * could not be opened.
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
      model.addVertex(vertex);
    } else if (type == "f") {
      std::string index1, index2, index3;
      stream >> index1 >> index2 >> index3;

      size_t i1 = parseFirstIndex(index1) - 1;
      size_t i2 = parseFirstIndex(index2) - 1;
      size_t i3 = parseFirstIndex(index3) - 1;

      model.addIndex(i1);
      model.addIndex(i2);
      model.addIndex(i3);
    }
  }

  model.setName(filename);

  file.close();

  return model;
}

/**
 * @brief Sends the model data to the GPU.
 *
 * This function generates OpenGL buffers for the vertex and index data of the
 * model and uploads the data to the GPU. It uses glGenBuffers, glBindBuffer,
 * and glBufferData to create and populate the buffers.
 */
void Model::sendModelToGPU() {
  glGenBuffers(1, &vertexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), vertices.data(),
               GL_STATIC_DRAW);
  glGenBuffers(1, &indexBuffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(uint32_t),
               indexes.data(), GL_STATIC_DRAW);
}

void Model::addVertex(vec3 vertex) { vertices.push_back(vertex); }

void Model::addIndex(uint32_t index) { indexes.push_back(index); }

/**
 * @brief Renders the model using OpenGL.
 *
 * This function binds the vertex and index buffers and issues a draw call to
 * render the model. It uses glBindBuffer to bind the buffers and glDrawElements
 * to draw the model using the index buffer.
 */
void Model::render() {
  glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
  glVertexPointer(3, GL_FLOAT, 0, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
  glDrawElements(GL_TRIANGLES, indexes.size(), GL_UNSIGNED_INT, 0);
}