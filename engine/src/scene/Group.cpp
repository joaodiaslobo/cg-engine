#include "Group.hpp"

#include "debug/Logger.hpp"
#include "math/Rotate.hpp"
#include "math/Scale.hpp"
#include "math/Translate.hpp"

static debug::Logger logger;

/**
 * @brief Renders all models and child groups within this group.
 *
 * This function iterates through all models in the current group and calls
 * their render function. It then iterates through all child groups and calls
 * their render function recursively.
 */
void Group::render() {
  glPushMatrix();

  applyTransformations(transformations);

  for (Model& model : models) {
    model.render();
  }

  for (Group& group : children) {
    group.render();
  }

  glPopMatrix();
}

void Group::clear() {
  models.clear();
  children.clear();
  transformations.clear();
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

  // Process transformations in order
  tinyxml2::XMLElement* transformElement =
      element->FirstChildElement("transform");
  if (transformElement != nullptr) {
    tinyxml2::XMLElement* transformation =
        transformElement->FirstChildElement();
    while (transformation != nullptr) {
      std::string tag = transformation->Value();

      if (tag == "translate") {
        float x = transformation->FloatAttribute("x");
        float y = transformation->FloatAttribute("y");
        float z = transformation->FloatAttribute("z");
        group.addTransformation(std::make_unique<Translate>(x, y, z));
      } else if (tag == "scale") {
        float x = transformation->FloatAttribute("x");
        float y = transformation->FloatAttribute("y");
        float z = transformation->FloatAttribute("z");
        group.addTransformation(std::make_unique<Scale>(x, y, z));
      } else if (tag == "rotate") {
        float angle = transformation->FloatAttribute("angle");
        float x = transformation->FloatAttribute("x");
        float y = transformation->FloatAttribute("y");
        float z = transformation->FloatAttribute("z");
        group.addTransformation(std::make_unique<Rotate>(angle, x, y, z));
      }

      transformation = transformation->NextSiblingElement();
    }
  }

  // Process models
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

  // Process child groups
  tinyxml2::XMLElement* groupElement = element->FirstChildElement("group");
  while (groupElement != nullptr) {
    group.addChild(initializeGroupFromXML(groupElement));
    groupElement = groupElement->NextSiblingElement("group");
  }

  return group;
}

/*
 * @brief Applies a list of transformations to the current model matrix.
 *
 * This function applies a list of transformations to the current model matrix
 * using OpenGL's glMultMatrixf function. The transformations are applied in
 * the order they are provided in the list.
 *
 * @param transformations A vector of unique pointers to Transformation objects
 *                        that represent the transformations to apply.
 */
void applyTransformations(
    const std::vector<std::unique_ptr<Transformation>>& transformations) {
  glm::mat4 modelMatrix = glm::mat4(1.0f);

  for (const std::unique_ptr<Transformation>& transformation :
       transformations) {
    modelMatrix = transformation->apply(modelMatrix);
  }

  glMultMatrixf(glm::value_ptr(modelMatrix));
}