#include "Engine.hpp"

static debug::Logger logger;

bool Engine::initialize() {
  DisplaySettings settings;
  settings.width = 800;
  settings.height = 600;
  settings.fullscreen = false;

  if (!window.initialize(&settings, "[CG ENGINE] - New Scene")) {
    return false;
  }

  configureGlfw(window);

  ui.initialize(&window);

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

  if (!window.initialize(&settings, ("[CG ENGINE] - " + filename).c_str())) {
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

  // Scene

  tinyxml2::XMLElement* rootGroupElement = root->FirstChildElement("group");

  if (rootGroupElement == nullptr) {
    logger.error("Failed to find root group element in file: " + filename);
    return false;
  }

  scene.setRoot(initializeGroupFromXML(rootGroupElement));

  configureGlfw(window);

  ui.initialize(&window);

  setupProjectionAndView();

  return true;
}

bool Engine::loadNewFile(const std::string& filename) {
  logger.info("Loading new file: " + filename);

  camera.setMode(CameraMode::STATIC);

  ui.shutdown();

  Window::terminate();

  scene.clear();

  if (!initializeFromFile(filename)) {
    logger.error("Failed to load new file: " + filename);
    return false;
  }

  logger.info("Successfully loaded new file: " + filename);

  run();

  return true;
}

void Engine::configureGlfw(Window& window) {
  glfwSetWindowUserPointer(window.getGlfwWindow(), this);
  glfwSetFramebufferSizeCallback(window.getGlfwWindow(),
                                 windowSizeUpdatedCallback);
  glfwSetKeyCallback(window.getGlfwWindow(), keyCallback);
  glfwSetCursorPosCallback(window.getGlfwWindow(), mouseCallback);
}

void Engine::run() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  double lastTime = glfwGetTime();

  while (!glfwWindowShouldClose(window.getGlfwWindow())) {
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    glfwPollEvents();

    camera.update(deltaTime);
    render();
    glfwSwapBuffers(window.getGlfwWindow());
  }

  Window::terminate();
}

void Engine::setupProjectionAndView() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspectRatio =
      static_cast<float>(window.width) / static_cast<float>(window.height);

  glViewport(0, 0, window.width, window.height);
  gluPerspective(camera.getFov(), aspectRatio, camera.getNear(),
                 camera.getFar());

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

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
  ui.render();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glLoadIdentity();

  camera.render();

  scene.render();

  renderSceneAxis();
  
  ui.postRender();
}

void windowSizeUpdatedCallback(GLFWwindow* window, int width, int height) {
  Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

  engine->getWindow()->setWindowSize(width, height);

  engine->setupProjectionAndView();
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                 int mods) {
  Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

  if (key == GLFW_KEY_T && action == GLFW_PRESS) {
    CameraMode currentMode = engine->getCamera()->getMode();
    if (currentMode == CameraMode::TURNTABLE) {
      engine->getCamera()->setMode(CameraMode::STATIC);
    } else {
      engine->getCamera()->setMode(CameraMode::TURNTABLE);
    }
  } else if (key == GLFW_KEY_F && action == GLFW_PRESS) {
    CameraMode currentMode = engine->getCamera()->getMode();
    if (currentMode == CameraMode::FREECAM) {
      engine->getCamera()->setMode(CameraMode::STATIC);
    } else {
      engine->getCamera()->setMode(CameraMode::FREECAM);
    }
  } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

  engine->getCamera()->processKeyboard(key, action);
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->getCamera()->processMouseMovement(xpos, ypos);
}