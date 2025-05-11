#include "Scene.hpp"

void Scene::render(ViewMode viewMode, bool showNormals) {
  root.render(time, showNormals, viewMode);
}

void Scene::clear() { root.clear(); }