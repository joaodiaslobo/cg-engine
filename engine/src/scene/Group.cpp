#include "Group.hpp"

#include "debug/Logger.hpp"

static debug::Logger logger;

void Group::render() {
  for (Model& model : models) {
    model.render();
  }

  for (Group& group : children) {
    group.render();
  }
}

Group initializeGroupFromXML(tinyxml2::XMLElement* element) {
  Group group;

  tinyxml2::XMLElement* modelsElement = element->FirstChildElement("models");
  if (modelsElement != nullptr) {
    tinyxml2::XMLElement* modelElement =
        modelsElement->FirstChildElement("model");
    while (modelElement != nullptr) {
      std::string filename = modelElement->Attribute("file");
      std::optional<Model> loadedModel = loadModel(filename);
      if (loadedModel.has_value()) {
        group.addModel(loadedModel.value());
      } else {
        logger.error("Failed to load model from file: " + filename + ".");
      }
      modelElement = modelElement->NextSiblingElement("model");
    }
  }
  return group;
}