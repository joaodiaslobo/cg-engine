#include "UI.hpp"

#include <tinyfiledialogs/tinyfiledialogs.h>

#include "../engine/Engine.hpp"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

void UI::initialize(Window* window) {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  this->io = &ImGui::GetIO();
  (void)io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window->getGlfwWindow(), true);
  ImGui_ImplOpenGL3_Init("#version 130");
  this->window = window;
  io->FontGlobalScale = 1.5f;
}

void UI::render() {
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

  ImGui::SetNextWindowPos(ImVec2(10, 40), ImGuiCond_Always);
  ImGui::Begin(
      "Performance", nullptr,
      ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize);
  ImGui::Text("FPS: %.1f", io->Framerate);
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