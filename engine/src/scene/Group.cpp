#include "Group.hpp"

#include "debug/Logger.hpp"

static debug::Logger logger;

/**
 * @brief Renders all models and child groups within this group.
 *
 * This function iterates through all models in the current group and calls
 * their render function. It then iterates through all child groups and calls
 * their render function recursively.
 */
void Group::render() {
  for (Model& model : models) {
    model.render();
  }

  for (Group& group : children) {
    group.render();
  }
}

void Group::clear() {
  models.clear();
  children.clear();
}

/**
 * @brief Initializes a Group object from an XML element.
 *
 * This function parses the provided XML element to extract model file paths,
 * loads the models, and adds them to a Group object. The XML element is
 * expected to have a structure where model file paths are specified within
 * <models> and <model> tags.
 *
 * @param element Pointer to the tinyxml2::XMLElement that contains the group
 *                initialization data.
 * @return A Group object initialized with the models specified in the XML
 * element.
 */
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