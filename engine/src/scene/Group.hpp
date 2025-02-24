#pragma once
#include <vector>

#include "Model.hpp"

using std::vector;

class Group {
 private:
  vector<Group> children;
  vector<Model> models;

 public:
  Group(){};
};