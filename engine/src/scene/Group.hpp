#pragma once
#include <tinyxml2.h>

#include <vector>

#include "Model.hpp"

using std::vector;

class Group {
 private:
  vector<Group> children;
  vector<Model> models;

 public:
  Group(){};
  void render();
  void addChild(Group child) { children.push_back(child); }
  void addModel(Model model) { models.push_back(model); }
  void clear();
};

Group initializeGroupFromXML(tinyxml2::XMLElement* element);