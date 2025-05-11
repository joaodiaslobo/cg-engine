#include "Engine.hpp"

static debug::Logger logger;

/**
 * @brief Initializes the engine.
 *
 * This function sets up the display settings, initializes the window with the
 * specified settings, configures GLFW for the window, and initializes the UI
 * system.
 *
 * @return true if the engine is successfully initialized, false otherwise.
 */
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

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  return true;
}

/**
 * @brief Initializes the engine from an XML file.
 *
 * This function loads and parses an XML file to initialize the engine's
 * settings, including window display settings, camera settings, and scene
 * graph.
 *
 * @param filename The path to the XML file to load.
 * @return true if the initialization is successful, false otherwise.
 */
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

  configureGlfw(window);

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

    if (cameraProjectionElement != nullptr) {
      float fov, near, far;
      cameraProjectionElement->QueryFloatAttribute("fov", &fov);
      cameraProjectionElement->QueryFloatAttribute("near", &near);
      cameraProjectionElement->QueryFloatAttribute("far", &far);

      camera.setFov(fov);
      camera.setNear(near);
      camera.setFar(far);
    }
  }

  // Scene

  // Lights

  tinyxml2::XMLElement* lightsElement = root->FirstChildElement("lights");

  if (lightsElement != nullptr) {
    for (tinyxml2::XMLElement* lightElement =
             lightsElement->FirstChildElement("light");
         lightElement != nullptr;
         lightElement = lightElement->NextSiblingElement("light")) {
      Light light;

      LightType type;
      std::string typeStr = lightElement->Attribute("type");

      if (typeStr == "directional") {
        type = LightType::DIRECTIONAL;
      } else if (typeStr == "point") {
        type = LightType::POINT;
      } else if (typeStr == "spot") {
        type = LightType::SPOTLIGHT;
      } else {
        logger.error("Unknown light type: " + typeStr);
        continue;
      }

      light.setType(type);

      if (type != LightType::DIRECTIONAL) {
        float x, y, z;
        lightElement->QueryFloatAttribute("posx", &x);
        lightElement->QueryFloatAttribute("posy", &y);
        lightElement->QueryFloatAttribute("posz", &z);
        light.setPosition(glm::vec3(x, y, z));
      }

      if (type != LightType::POINT) {
        float x, y, z;
        lightElement->QueryFloatAttribute("dirx", &x);
        lightElement->QueryFloatAttribute("diry", &y);
        lightElement->QueryFloatAttribute("dirz", &z);
        light.setDirection(glm::vec3(x, y, z));
      }

      if (type == LightType::SPOTLIGHT) {
        float cutoff;
        lightElement->QueryFloatAttribute("cutoff", &cutoff);
        light.setCutoff(cutoff);
      }

      tinyxml2::XMLElement* colorElement =
          lightElement->FirstChildElement("color");
      if (colorElement != nullptr) {
        float r, g, b;
        colorElement->QueryFloatAttribute("R", &r);
        colorElement->QueryFloatAttribute("G", &g);
        colorElement->QueryFloatAttribute("B", &b);
        r /= 255.0f;
        g /= 255.0f;
        b /= 255.0f;
        light.setColor(glm::vec3(r, g, b));
      } else {
        // Set default color
        light.setColor(glm::vec3(1.0f, 1.0f, 1.0f));
      }

      scene.addLight(light);
    }
  }

  // Setup lights
  for (int i = 0; i < 8; ++i) {
    glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, 180);
    glDisable(GL_LIGHT0 + i);
  }

  tinyxml2::XMLElement* rootGroupElement = root->FirstChildElement("group");

  if (rootGroupElement == nullptr) {
    logger.error("Failed to find root group element in file: " + filename);
    return false;
  }
  scene.setRoot(initializeGroupFromXML(rootGroupElement));

  ui.initialize(&window);

  setupProjectionAndView();

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  return true;
}

/**
 * @brief Loads a new file and initializes the engine with its contents.
 *
 * @param filename The path to the file to be loaded.
 * @return true if the file was successfully loaded and initialized, false
 * otherwise.
 */
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

/**
 * @brief Configures GLFW callbacks for the given window.
 *
 * This function sets up various GLFW callbacks for the provided window,
 * including user pointer, framebuffer size, key input, and cursor position.
 *
 * @param window The window for which to configure GLFW callbacks.
 */
void Engine::configureGlfw(Window& window) {
  glfwSetErrorCallback(glfwErrorCallback);
  glfwSetWindowUserPointer(window.getGlfwWindow(), this);
  glfwSetFramebufferSizeCallback(window.getGlfwWindow(),
                                 windowSizeUpdatedCallback);
  glfwSetKeyCallback(window.getGlfwWindow(), keyCallback);
  glfwSetCursorPosCallback(window.getGlfwWindow(), mouseCallback);

  if (glewInit() != GLEW_OK) {
    logger.error("Failed to initialize GLEW.");
  }
}

/**
 * @brief Runs the main loop of the engine.
 *
 * This function initializes necessary OpenGL states and enters the main loop
 * where it continuously updates the camera, renders the scene, and swaps the
 * buffers until the window should close. It also handles polling of window
 * events.
 *
 */
void Engine::run() {
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnable(GL_RESCALE_NORMAL);

  constexpr float amb[4] = {1.0f, 1.0f, 1.0f, 1.0f};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

  double lastTime = glfwGetTime();

  while (!glfwWindowShouldClose(window.getGlfwWindow())) {
    double currentTime = glfwGetTime();
    float deltaTime = static_cast<float>(currentTime - lastTime);
    lastTime = currentTime;

    glfwPollEvents();

    camera.update(deltaTime);
    scene.updateTime(deltaTime);

    render();
    glfwSwapBuffers(window.getGlfwWindow());
  }

  ui.terminate();
  Window::terminate();
}

/**
 * @brief Sets up the projection and view matrices for the rendering engine.
 *
 * This function configures the OpenGL projection and view matrices based on
 * the current window dimensions and camera settings. It sets the projection
 * matrix to a perspective projection using the camera's field of view, aspect
 * ratio, near clipping plane, and far clipping plane.
 *
 */
void Engine::setupProjectionAndView() {
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  float aspectRatio =
      static_cast<float>(window.width) / static_cast<float>(window.height);

  glViewport(0, 0, window.width, window.height);
  gluPerspective(camera.getFov(), aspectRatio, camera.getNear(),
                 camera.getFar());

  glMatrixMode(GL_MODELVIEW);
}

/**
 * @brief Renders the coordinate axes in the scene.
 *
 * This function draws the x-axis, y-axis, and z-axis using OpenGL lines.
 * Each axis is colored differently for easy identification:
 * - x-axis: red
 * - y-axis: green
 * - z-axis: blue
 *
 * The axes extend from -1000 to 1000 units in their respective directions.
 */
void Engine::renderSceneAxis() {
  disableLightRendering();
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
  maybeEnableLightRendering();
}

/**
 * @brief Renders the entire scene including UI, camera, and scene objects.
 */
void Engine::render() {
  ui.render();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glLoadIdentity();

  camera.render();

  if (settings.getViewmode() == SHADED) {
    renderLights();
  }

  scene.render(settings.getViewmode(), settings.getShowNormals());

  renderSceneAxis();

  ui.postRender();
}

/**
 * @brief Callback function to handle window size updates.
 *
 * This function is called whenever the window size is changed. It updates the
 * window size in the engine and reconfigures the projection and view settings.
 *
 * @param window Pointer to the GLFW window that received the event.
 * @param width New width of the window.
 * @param height New height of the window.
 */
void windowSizeUpdatedCallback(GLFWwindow* window, int width, int height) {
  Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));

  engine->getWindow()->setWindowSize(width, height);

  engine->setupProjectionAndView();
}

/**
 * @brief Disables light rendering in the OpenGL context.
 *
 * This function disables the lighting feature in OpenGL, which is useful when
 * rendering in wireframe or flat shading modes where lighting is not needed.
 */
void Engine::disableLightRendering() { glDisable(GL_LIGHTING); }

/**
 * @brief Enables light rendering in the OpenGL context if the current view mode
 * is SHADED.
 *
 * This function enables the lighting feature in OpenGL, which is necessary for
 * rendering objects with shading. It checks the current view mode and only
 * enables lighting if the mode is set to SHADED.
 */
void Engine::maybeEnableLightRendering() {
  if (settings.getViewmode() == SHADED) {
    glEnable(GL_LIGHTING);
  }
}

/**
 * @brief Callback function for handling key press events.
 *
 * This function is called whenever a key is pressed, released, or held down.
 * It handles specific key events to change the camera mode or close the window.
 *
 * @param window The GLFW window that received the event.
 * @param key The keyboard key that was pressed or released.
 * @param scancode The system-specific scancode of the key.
 * @param action The action (GLFW_PRESS, GLFW_RELEASE, or GLFW_REPEAT).
 * @param mods Bit field describing which modifier keys were held down.
 *
 * The function performs the following actions based on the key and action:
 * - If the 'T' key is pressed, it toggles the camera mode between TURNTABLE and
 * STATIC.
 * - If the 'F' key is pressed, it toggles the camera mode between FREECAM and
 * STATIC.
 * - If the 'ESCAPE' key is pressed, it sets the window to close.
 *
 * Additionally, it calls the processKeyboard method of the camera to handle
 * other key events.
 */
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
  } else if (key == GLFW_KEY_U && action == GLFW_PRESS) {
    engine->getUI()->toggleUI();
  } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } else if (key == GLFW_KEY_N && action == GLFW_PRESS) {
    engine->getSettings()->toggleNormals();
  } else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
    engine->getSettings()->toggleViewmode();
    switch (engine->getSettings()->getViewmode()) {
      case WIREFRAME:
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glDisable(GL_LIGHTING);
        break;
      case FLAT:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_LIGHTING);
        break;
      case SHADED:
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glEnable(GL_LIGHTING);
        break;
      default:
        break;
    }
  }

  engine->getCamera()->processKeyboard(key, action);
}

/**
 * @brief Callback function to handle mouse movement events.
 *
 * This function is called whenever the mouse is moved within the GLFW window.
 * It retrieves the Engine instance associated with the window and delegates
 * the mouse movement processing to the camera.
 *
 * @param window Pointer to the GLFW window that received the event.
 * @param xpos The new x-coordinate of the cursor.
 * @param ypos The new y-coordinate of the cursor.
 */
void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
  Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
  engine->getCamera()->processMouseMovement(xpos, ypos);
}

void glfwErrorCallback(const int error, const char* description) {
  logger.error("GLFW Error: " + std::to_string(error) + ": " +
               std::string(description));
}

/**
 * @brief Renders all lights in the scene.
 *
 * This function iterates through all lights in the scene and calls their
 * render method to display them in the OpenGL context.
 */
void Engine::renderLights() {
  int lightIndex = 0;
  disableLightRendering();
  for (auto& light : scene.getLights()) {
    light.render(lightIndex);
    lightIndex++;
  }
  maybeEnableLightRendering();
}