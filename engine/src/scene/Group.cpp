#include "Group.hpp"

#include "debug/Logger.hpp"
#include "math/Path.hpp"
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
void Group::render(float time, bool renderNormals) {
  glPushMatrix();

  applyTransformations(transformations, time);

  for (Model& model : models) {
    model.render();
    if (renderNormals) {
      model.renderNormals(2.0f);
    }
  }

  for (Group& group : children) {
    group.render(time, renderNormals);
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
  static int groupCounter = -1;
  Group group;

  const char* name = element->Attribute("name");
  if (name != nullptr) {
    group.setName(name);
  } else {
    if (groupCounter <= 0) {
      group.setName("Group");
      groupCounter++;
    } else {
      group.setName("Group (" + std::to_string(groupCounter++) + ")");
    }
  }

  // Process transformations in order
  tinyxml2::XMLElement* transformElement =
      element->FirstChildElement("transform");
  if (transformElement != nullptr) {
    tinyxml2::XMLElement* transformation =
        transformElement->FirstChildElement();
    while (transformation != nullptr) {
      std::string tag = transformation->Value();

      if (tag == "translate") {
        if (transformation->Attribute("time") != nullptr) {
          // Path based translation
          float duration = transformation->FloatAttribute("time");
          std::vector<vec3> path;
          for (tinyxml2::XMLElement* point =
                   transformation->FirstChildElement("point");
               point != nullptr; point = point->NextSiblingElement("point")) {
            float x = point->FloatAttribute("x");
            float y = point->FloatAttribute("y");
            float z = point->FloatAttribute("z");
            path.push_back(vec3(x, y, z));
          }

          bool align = false;

          if (transformation->Attribute("align") != nullptr) {
            align = transformation->BoolAttribute("align");
          }

          bool render_path = false;

          if (transformation->Attribute("render_path") != nullptr) {
            render_path = transformation->BoolAttribute("render_path");
          }

          group.addTransformation(
              std::make_unique<Path>(duration, align, path, render_path));
        } else {
          // Normal translation
          float x = transformation->FloatAttribute("x");
          float y = transformation->FloatAttribute("y");
          float z = transformation->FloatAttribute("z");
          group.addTransformation(std::make_unique<Translate>(x, y, z));
        }
      } else if (tag == "scale") {
        float x = transformation->FloatAttribute("x");
        float y = transformation->FloatAttribute("y");
        float z = transformation->FloatAttribute("z");
        group.addTransformation(std::make_unique<Scale>(x, y, z));
      } else if (tag == "rotate") {
        float angle = 0.0f;
        float duration = 0.0f;

        // Can either have angle or time
        if (transformation->Attribute("time") != nullptr) {
          duration = transformation->FloatAttribute("time");
        } else if (transformation->Attribute("angle") != nullptr) {
          angle = transformation->FloatAttribute("angle");
        } else {
          logger.error("No angle or time attribute found for rotate (" +
                       group.getName() + ").");
        }

        float x = transformation->FloatAttribute("x");
        float y = transformation->FloatAttribute("y");
        float z = transformation->FloatAttribute("z");

        group.addTransformation(
            std::make_unique<Rotate>(angle, duration, x, y, z));
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
        // Maybe load material data

        tinyxml2::XMLElement* materialElement =
            modelElement->FirstChildElement("color");

        if (materialElement != nullptr) {
          tinyxml2::XMLElement* diffuseElement =
              materialElement->FirstChildElement("diffuse");
          tinyxml2::XMLElement* ambientElement =
              materialElement->FirstChildElement("ambient");
          tinyxml2::XMLElement* specularElement =
              materialElement->FirstChildElement("specular");
          tinyxml2::XMLElement* emissionElement =
              materialElement->FirstChildElement("emissive");
          tinyxml2::XMLElement* shininessElement =
              materialElement->FirstChildElement("shininess");

          Material material;
          if (diffuseElement != nullptr) {
            diffuseElement->QueryFloatAttribute("R", &material.diffuse.r);
            diffuseElement->QueryFloatAttribute("G", &material.diffuse.g);
            diffuseElement->QueryFloatAttribute("B", &material.diffuse.b);
            material.diffuse.r /= 255.0f;
            material.diffuse.g /= 255.0f;
            material.diffuse.b /= 255.0f;
          }
          if (ambientElement != nullptr) {
            ambientElement->QueryFloatAttribute("R", &material.ambient.r);
            ambientElement->QueryFloatAttribute("G", &material.ambient.g);
            ambientElement->QueryFloatAttribute("B", &material.ambient.b);
            material.ambient.r /= 255.0f;
            material.ambient.g /= 255.0f;
            material.ambient.b /= 255.0f;
          }
          if (specularElement != nullptr) {
            specularElement->QueryFloatAttribute("R", &material.specular.r);
            specularElement->QueryFloatAttribute("G", &material.specular.g);
            specularElement->QueryFloatAttribute("B", &material.specular.b);
            material.specular.r /= 255.0f;
            material.specular.g /= 255.0f;
            material.specular.b /= 255.0f;
          }
          if (emissionElement != nullptr) {
            emissionElement->QueryFloatAttribute("R", &material.emission.r);
            emissionElement->QueryFloatAttribute("G", &material.emission.g);
            emissionElement->QueryFloatAttribute("B", &material.emission.b);
            material.emission.r /= 255.0f;
            material.emission.g /= 255.0f;
            material.emission.b /= 255.0f;
          }
          if (shininessElement != nullptr) {
            shininessElement->QueryFloatAttribute("value", &material.shininess);
            material.shininess /= 255.0f;
          }

          loadedModel.value().setMaterial(material);
        }

        loadedModel.value().sendModelToGPU();
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
    const std::vector<std::unique_ptr<Transformation>>& transformations,
    float time) {
  glm::mat4 modelMatrix = glm::mat4(1.0f);

  for (const std::unique_ptr<Transformation>& transformation :
       transformations) {
    modelMatrix = transformation->apply(modelMatrix, time);
  }

  glMultMatrixf(glm::value_ptr(modelMatrix));
}