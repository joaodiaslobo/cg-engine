#include "UI.hpp"

#include "../engine/Engine.hpp"

void LoadMainFont(ImGuiIO& io) {
  ImFontConfig config;
  config.PixelSnapH = true;
  io.Fonts->AddFontFromFileTTF("engine/assets/fonts/robotomono.ttf", 16.0f);
}

void LoadIconFont(ImGuiIO& io) {
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig config;
  config.MergeMode = true;
  config.PixelSnapH = true;

  io.Fonts->AddFontFromFileTTF("engine/assets/fonts/fontawesome.otf", 16.0f,
                               &config, icon_ranges);
}

void UI::DrawGroupTree(const Group& group, const std::string& name,
                       NodeType type) {
  if (SceneTreeNode(name.c_str(), type, true, type == NodeType::WORLD)) {
    if (type == NodeType::WORLD) {
      // If this is the world node, add the camera
      SceneTreeNode("Camera", NodeType::CAMERA, false, false);

      Engine* engine = static_cast<Engine*>(
          glfwGetWindowUserPointer(window->getGlfwWindow()));
      if (engine) {
        Scene* scene = engine->getScene();
        if (scene) {
          // Maybe add lights
          if (!scene->getLights().empty()) {
            if (SceneTreeNode("Lights", NodeType::LIGHTS, true, false)) {
              int pointCount = 0, dirCount = 0, spotCount = 0;

              for (auto& light : scene->getLights()) {
                std::string label;
                NodeType nodeType;

                switch (light.getType()) {
                  case LightType::DIRECTIONAL:
                    label = dirCount++ == 0
                                ? "Directional Light"
                                : "Directional Light (" +
                                      std::to_string(dirCount) + ")";
                    nodeType = NodeType::LIGHT_DIRECTIONAL;
                    break;
                  case LightType::POINT:
                    label = pointCount++ == 0
                                ? "Point Light"
                                : "Point Light (" + std::to_string(pointCount) +
                                      ")";
                    nodeType = NodeType::LIGHT_POINT;
                    break;
                  case LightType::SPOTLIGHT:
                    label =
                        spotCount++ == 0
                            ? "Spot Light"
                            : "Spot Light (" + std::to_string(spotCount) + ")";
                    nodeType = NodeType::LIGHT_SPOT;
                    break;
                  default:
                    continue;
                }

                SceneTreeNode(label.c_str(), nodeType, false, false);
              }

              ImGui::TreePop();
            }
          }
        }
      }
    }

    for (auto& child : group.getChildren()) {
      DrawGroupTree(child, child.getName(), NodeType::GROUP);
    }

    for (auto& model : group.getModels()) {
      SceneTreeNode(model.getName().c_str(), NodeType::MODEL, false, false);
    }
    ImGui::TreePop();
  }
}

void UI::initialize(Window* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  this->io = &ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
  LoadMainFont(*io);
  LoadIconFont(*io);
  this->window = window;
  io->FontGlobalScale = 1.5f;
  io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
}

void UI::terminate() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

void UI::toggleUI() { enabled = !enabled; }

void UI::render() {
  if (!enabled) {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    ImGui::Render();
    return;
  }

  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (ImGui::BeginMainMenuBar()) {
    if (ImGui::BeginMenu("File")) {
      if (ImGui::MenuItem("Open")) {
        const char* filters[] = {"*.xml"};
        const char* file = tinyfd_openFileDialog("Open XML Scene File", "", 1,
                                                 filters, "XML files", 0);
        if (file) {
          Engine* engine = static_cast<Engine*>(
              glfwGetWindowUserPointer(window->getGlfwWindow()));
          engine->loadNewFile(file);
        }
      }
      ImGui::EndMenu();
    }
    ImGui::EndMainMenuBar();
  }

  ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(),
                               ImGuiDockNodeFlags_PassthruCentralNode);

  ImGui::SetNextWindowPos(ImVec2(ImGui::GetIO().DisplaySize.x - 30,
                                 ImGui::GetIO().DisplaySize.y - 30),
                          ImGuiCond_Always, ImVec2(1.0f, 1.0f));
  ImGui::Begin(
      "Performance", nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("FPS: %.1f", io->Framerate);
  ImGui::End();

  ImGui::Begin("Inspector");
  Engine* engine =
      static_cast<Engine*>(glfwGetWindowUserPointer(window->getGlfwWindow()));
  if (engine) {
    Scene* scene = engine->getScene();
    if (scene) {
      DrawGroupTree(scene->getRoot(), "World", NodeType::WORLD);
    }
  }
  ImGui::End();

  ImGui::Render();
}

void UI::postRender() {
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::shutdown() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

bool SceneTreeNode(const char* label, NodeType type, bool hasChildren,
                   bool startOpen) {
  ImGuiContext& g = *ImGui::GetCurrentContext();
  ImGuiWindow* window = g.CurrentWindow;

  ImGuiID id = window->GetID(label);
  ImVec2 pos = window->DC.CursorPos;

  ImRect bb(pos, ImVec2(pos.x + ImGui::GetContentRegionAvail().x,
                        pos.y + g.FontSize));

  int* stored_open = window->DC.StateStorage->GetIntRef(id, startOpen ? 1 : 0);
  bool opened = (*stored_open != 0);
  bool hovered, held;

  if (ImGui::ButtonBehavior(bb, id, &hovered, &held,
                            ImGuiButtonFlags_PressedOnClick))
    window->DC.StateStorage->SetInt(id, opened ? 0 : 1);

  if (hovered || held)
    window->DrawList->AddRectFilled(
        bb.Min, bb.Max,
        ImGui::GetColorU32(held ? ImGuiCol_HeaderActive
                                : ImGuiCol_HeaderHovered));

  const char* icon = (type == NodeType::WORLD)               ? ICON_FA_WORLD
                     : (type == NodeType::MODEL)             ? ICON_FA_CUBE
                     : (type == NodeType::CAMERA)            ? ICON_FA_CAMERA
                     : (type == NodeType::LIGHTS)            ? ICON_FA_LIGHTBULB
                     : (type == NodeType::LIGHT_DIRECTIONAL) ? ICON_FA_SUN
                     : (type == NodeType::LIGHT_POINT)       ? ICON_FA_LIGHTBULB
                     : (type == NodeType::LIGHT_SPOT)
                         ? ICON_FA_SPOTLIGHT
                         : (opened ? ICON_FA_FOLDER_OPEN : ICON_FA_FOLDER);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4.0f, 0.0f));
  ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, 10.0f);

  ImGui::Text("%s  %s", icon, label);

  ImGui::ItemSize(bb, 0.0f);
  ImGui::ItemAdd(bb, id);

  ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9.0f);

  ImGui::PopStyleVar(3);

  if (opened && hasChildren) ImGui::TreePush(label);

  return opened;
}
