#pragma once

#include <GL/glew.h>
#define GLFW_INCLUDE_GLU
#include <GLFW/glfw3.h>
#include <imgui_internal.h>
#include <tinyfiledialogs/tinyfiledialogs.h>

#include "../scene/Scene.hpp"
#include "../window/Window.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#define ICON_MIN_FA 0xf000
#define ICON_MAX_FA 0xf3ff
#define ICON_FA_WORLD ""
#define ICON_FA_CUBE ""
#define ICON_FA_CAMERA ""
#define ICON_FA_FOLDER "\xef\x81\xbb"
#define ICON_FA_FOLDER_OPEN "\xef\x81\xbc"
#define ICON_FA_LIGHTBULB ""
#define ICON_FA_SUN ""
#define ICON_FA_SPOTLIGHT ""
#define ICON_FA_PAUSE ""
#define ICON_FA_PLAY ""

enum class NodeType {
  WORLD,
  CAMERA,
  GROUP,
  MODEL,
  LIGHTS,
  LIGHT_DIRECTIONAL,
  LIGHT_POINT,
  LIGHT_SPOT
};

class UI {
 public:
 private:
  Window* window;
  float fps = 0.0f;
  ImGuiIO* io = nullptr;
  bool enabled = true;
  bool showOptionsWindow = false;

 public:
  void toggleUI();
  void initialize(Window* window);
  void terminate();
  void render();
  void postRender();
  void shutdown();
  void setFPS(float fps) { this->fps = fps; }
  void DrawGroupTree(const Group& group, const std::string& name,
                     NodeType type);
};

void LoadMainFont(ImGuiIO& io);
void LoadIconFont(ImGuiIO& io);
bool SceneTreeNode(const char* label, NodeType type, bool hasChildren,
                   bool startOpen);