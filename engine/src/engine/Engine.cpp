#include "Engine.hpp"

static debug::Logger logger;

bool Engine::initialize() {
  DisplaySettings settings;
  settings.width = 800;
  settings.height = 600;
  settings.fullscreen = false;

  if (!window.initialize(&settings)) {
    return false;
  }

  updateViewport();

  return true;
}

bool Engine::initializeFromFile(const string& filename) {
  tinyxml2::XMLDocument doc;

  if (doc.LoadFile(filename.c_str()) != tinyxml2::XML_SUCCESS) {
    logger.error("Failed to load file: " + filename);
    return false;
  }

  tinyxml2::XMLElement* root = doc.FirstChildElement("world");

  if (root == nullptr) {
    logger.error("Failed to find root element in file: " + filename);
    return false;
  }

  // Window

  tinyxml2::XMLElement* windowElement = root->FirstChildElement("window");

  if (windowElement == nullptr) {
    logger.error("Failed to find window element in file: " + filename);
    return false;
  }

  DisplaySettings settings;
  windowElement->QueryIntAttribute("width", &settings.width);
  windowElement->QueryIntAttribute("height", &settings.height);
  settings.fullscreen = false;

  if (!window.initialize(&settings)) {
    return false;
  }

  // Camera

  tinyxml2::XMLElement* cameraElement = root->FirstChildElement("camera");

  if (cameraElement != nullptr) {
    // Camera settings
    tinyxml2::XMLElement* cameraPositionElement =
        cameraElement->FirstChildElement("position");
    tinyxml2::XMLElement* cameraLookingAtElement =
        cameraElement->FirstChildElement("lookAt");
    tinyxml2::XMLElement* cameraUpElement =
        cameraElement->FirstChildElement("up");
    tinyxml2::XMLElement* cameraProjectionElement =
        cameraElement->FirstChildElement("projection");

    if (cameraPositionElement != nullptr) {
      float x, y, z;
      cameraPositionElement->QueryFloatAttribute("x", &x);
      cameraPositionElement->QueryFloatAttribute("y", &y);
      cameraPositionElement->QueryFloatAttribute("z", &z);

      camera.setPosition(glm::vec3(x, y, z));
    }

    if (cameraLookingAtElement != nullptr) {
      float x, y, z;
      cameraLookingAtElement->QueryFloatAttribute("x", &x);
      cameraLookingAtElement->QueryFloatAttribute("y", &y);
      cameraLookingAtElement->QueryFloatAttribute("z", &z);

      camera.setLookingAt(glm::vec3(x, y, z));
    }

    if (cameraUpElement != nullptr) {
      float x, y, z;
      cameraUpElement->QueryFloatAttribute("x", &x);
      cameraUpElement->QueryFloatAttribute("y", &y);
      cameraUpElement->QueryFloatAttribute("z", &z);

      camera.setUp(glm::vec3(x, y, z));
    }
  }

  updateViewport();

  return true;
}

void Engine::run() {
  while (!glfwWindowShouldClose(window.getGlfwWindow())) {
    glfwPollEvents();

    render();
    glfwSwapBuffers(window.getGlfwWindow());
  }

  Window::terminate();
}

void Engine::updateViewport() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glViewport(0, 0, window.width, window.height);
  gluPerspective(camera.getFov(), window.width / window.height,
                 camera.getNear(), camera.getFar());

  glMatrixMode(GL_MODELVIEW);
}

void renderSceneAxis() {
  glBegin(GL_LINES);

  // x-axis
  glColor3f(1.0, 0.0, 0.0);
  glVertex3f(-1000.0, 0.0, 0.0);
  glVertex3f(1000.0, 0.0, 0.0);

  // y-axis
  glColor3f(0.0, 1.0, 0.0);
  glVertex3f(0.0, -1000.0, 0.0);
  glVertex3f(0.0, 1000.0, 0.0);

  // z-axis
  glColor3f(0.0, 0.0, 1.0);
  glVertex3f(0.0, 0.0, -1000.0);
  glVertex3f(0.0, 0.0, 1000.0);

  glColor3f(1.0, 1.0, 1.0);

  glEnd();
}

void Engine::render() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glLoadIdentity();

  camera.render();

  renderSceneAxis();
}