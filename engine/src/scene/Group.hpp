#pragma once
#include <GLFW/glfw3.h>
#include <tinyxml2.h>

#include <glm/gtc/type_ptr.hpp>
#include <memory>
#include <vector>

#include "Model.hpp"
#include "math/Transformation.hpp"

using std::unique_ptr;
using std::vector;

class Group {
 private:
  string name;
  vector<Group> children;
  vector<Model> models;
  vector<std::unique_ptr<Transformation>> transformations;

 public:
  Group() = default;
  Group(const Group&) = delete;
  Group& operator=(const Group&) = delete;
  Group(Group&&) = default;
  Group& operator=(Group&&) = default;

  void render();
  void setName(string name) { this->name = name; }
  string getName() const { return name; }
  void addChild(Group child) { children.push_back(std::move(child)); }
  void addModel(Model model) { models.push_back(std::move(model)); }
  void addTransformation(std::unique_ptr<Transformation> transformation) {
    transformations.push_back(std::move(transformation));
  }
  void clear();
  const vector<Group>& getChildren() const { return children; }
  const vector<Model>& getModels() const { return models; }
};

Group initializeGroupFromXML(tinyxml2::XMLElement* element);
void applyTransformations(
    const std::vector<std::unique_ptr<Transformation>>& transformations);