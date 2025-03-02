#include "Model.hpp"

#include <GLFW/glfw3.h>

#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>

#include "debug/Logger.hpp"

using std::optional;

static debug::Logger logger;

/**
 * @brief Loads a model from a file.
 *
 * This function attempts to load a model from the specified file. It supports
 * files with the extensions ".3d" and ".obj". If the file extension is not
 * recognized or the file cannot be loaded, an error is logged and an empty
 * optional is returned.
 *
 * @param filename The path to the model file.
 * @return An optional containing the loaded model if successful, or an empty
 * optional if the model could not be loaded.
 */
optional<Model> loadModel(const string& filename) {
  if (filename.size() >= 3 &&
      filename.compare(filename.size() - 3, 3, ".3d") == 0) {
    return load3dModel(filename);
  } else if (filename.size() >= 4 &&
             filename.compare(filename.size() - 4, 4, ".obj") == 0) {
    return loadObjModel(filename);
  }

  logger.error("Failed to load model: " + filename);
  return {};
}

/**
 * @brief Parses the first index from a given string_view.
 *
 * This function extracts the first integer from a string_view that may contain
 * a '/' character. If the '/' character is found, the function converts the
 * substring before the '/' to an integer. If the '/' character is not found,
 * the entire string is converted to an integer.
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
 * @brief Loads a 3D model from a file.
 *
 * This function reads a 3D model from the specified file and returns it as an
 * optional Model object. If the file cannot be opened, an empty optional is
 * returned.
 *
 * @param filename The path to the file containing the 3D model data.
 * @return optional<Model> The loaded 3D model, or an empty optional if the file
 * could not be opened.
 */
optional<Model> load3dModel(const string& filename) {
  Model model;

  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string type;
    iss >> type;

    if (type == "v") {
      vec3 vertex;
      iss >> vertex.x >> vertex.y >> vertex.z;
      model.addVertex(vertex);
    }
  }

  model.setName(filename);

  file.close();

  return model;
}

/**
 * @brief Loads an OBJ model from a file.
 *
 * This function reads an OBJ file specified by the filename and constructs a
 * Model object from the vertex and face data contained in the file. The
 * function returns an optional Model object, which will be empty if the file
 * could not be opened.
 *
 * @param filename The path to the OBJ file to be loaded.
 * @return An optional Model object containing the loaded model data. If the
 * file could not be opened, an empty optional is returned.
 */
optional<Model> loadObjModel(const string& filename) {
  Model model;

  std::ifstream file(filename);
  if (!file.is_open()) {
    logger.error("Failed to open file: " + filename);
    return {};
  }

  string line;
  vector<vec3> global_vertices;

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
      global_vertices.push_back(vertex);
    } else if (type == "f") {
      std::string vertex1, vertex2, vertex3;
      stream >> vertex1 >> vertex2 >> vertex3;

      size_t i1 = parseFirstIndex(vertex1) - 1;
      size_t i2 = parseFirstIndex(vertex2) - 1;
      size_t i3 = parseFirstIndex(vertex3) - 1;

      model.addVertex(global_vertices[i1]);
      model.addVertex(global_vertices[i2]);
      model.addVertex(global_vertices[i3]);
    }
  }

  model.setName(filename);

  file.close();

  return model;
}

void Model::addVertex(vec3 vertex) { vertices.push_back(vertex); }

/**
 * @brief Renders the model using OpenGL.
 *
 * This function iterates over the vertices of the model and renders them
 * as triangles using the OpenGL glBegin(GL_TRIANGLES) and glEnd() functions.
 */
void Model::render() {
  glBegin(GL_TRIANGLES);

  for (const vec3& vertex : vertices) {
    glVertex3f(vertex.x, vertex.y, vertex.z);
  }

  glEnd();
}