#pragma once
#include <tinyxml2.h>

#include <memory>
#include <vector>

#include "Model.hpp"
#include "math/Transformation.hpp"

using std::unique_ptr;
using std::vector;

class Group {
 private:
  vector<Group> children;
  vector<Model> models;
  vector<Transformation> transformations;

 public:
  Group(){};
  void render();
  void addChild(Group child) { children.push_back(child); }
  void addModel(Model model) { models.push_back(model); }
  void addTransformation(const Transformation& transformation) {
    transformations.push_back(transformation);
  }
  void clear();
  const vector<Group>& getChildren() const { return children; }
  const vector<Model>& getModels() const { return models; }
};

Group initializeGroupFromXML(tinyxml2::XMLElement* element);