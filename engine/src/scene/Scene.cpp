#include "Scene.hpp"

void Scene::render(bool showNormals) { root.render(time, showNormals); }

void Scene::clear() { root.clear(); }