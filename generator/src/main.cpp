#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Generator.hpp"

void printUsage() {
  std::cout
      << "Usage:\n"
      << "  generator sphere <radius> <slices> <stacks> <output_file>\n"
      << "  generator box <length> <divisions> <output_file>\n"
      << "  generator cone <radius> <height> <slices> <stacks> <output_file>\n"
      << "  generator plane <length> <divisions> <output_file>\n"
      << "  generator cylinder <radius> <height> <slices> <stacks> "
         "<output_file>\n"
      << "  generator torus <radius> <tube_radius> <slices> <stacks> "
         "<output_file>\n"
      << " generator icosphere <radius> <subdivisions> <output_file>\n"
      << " generator patch <patch_file> <tessellation> <output_file>\n";
}

void handleSphere(const std::vector<std::string>& args) {
  std::cout << "Generating sphere with radius " << args[1] << ", slices "
            << args[2] << ", stacks " << args[3] << " | Output: " << args[4]
            << std::endl;

  float radius = std::stof(args[1]);
  int slices = std::stoi(args[2]);
  int stacks = std::stoi(args[3]);

  Model model = generator::Sphere(radius, slices, stacks);
  generator::Export(model, args[4]);
}

void handleBox(const std::vector<std::string>& args) {
  std::cout << "Generating box with length " << args[1] << ", divisions "
            << args[2] << " | Output: " << args[3] << std::endl;

  float size = std::stof(args[1]);
  int divisions = std::stoi(args[2]);

  Model model = generator::Box(size, divisions);
  generator::Export(model, args[3]);
}

void handleCone(const std::vector<std::string>& args) {
  std::cout << "Generating cone with radius " << args[1] << ", height "
            << args[2] << ", slices " << args[3] << ", stacks " << args[4]
            << " | Output: " << args[5] << std::endl;

  float radius = std::stof(args[1]);
  float height = std::stof(args[2]);
  int slices = std::stoi(args[3]);
  int stacks = std::stoi(args[4]);

  Model model = generator::Cone(radius, height, slices, stacks);
  generator::Export(model, args[5]);
}

void handlePlane(const std::vector<std::string>& args) {
  std::cout << "Generating plane with length " << args[1] << ", divisions "
            << args[2] << " | Output: " << args[3] << std::endl;

  float length = std::stof(args[1]);
  int divisions = std::stoi(args[2]);

  Model model = generator::Plane(length, divisions);
  generator::Export(model, args[3]);
}

void handleCylinder(const std::vector<std::string>& args) {
  std::cout << "Generating cylinder with radius " << args[1] << ", height "
            << args[2] << ", slices " << args[3] << ", stacks " << args[4]
            << " | Output: " << args[5] << std::endl;

  float radius = std::stof(args[1]);
  float height = std::stof(args[2]);
  int slices = std::stoi(args[3]);
  int stacks = std::stoi(args[4]);

  Model model = generator::Cylinder(radius, height, slices, stacks);
  generator::Export(model, args[5]);
}

void handleTorus(const std::vector<std::string>& args) {
  std::cout << "Generating torus with radius " << args[1] << ", tube radius "
            << args[2] << ", slices " << args[3] << ", stacks " << args[4]
            << " | Output: " << args[5] << std::endl;

  float radius = std::stof(args[1]);
  float tubeRadius = std::stof(args[2]);
  int slices = std::stoi(args[3]);
  int stacks = std::stoi(args[4]);

  Model model = generator::Torus(radius, tubeRadius, slices, stacks);
  generator::Export(model, args[5]);
}

void handleIcosphere(const std::vector<std::string>& args) {
  std::cout << "Generating icosphere with radius " << args[1]
            << ", subdivisions " << args[2] << " | Output: " << args[3]
            << std::endl;

  float radius = std::stof(args[1]);
  float subdivisions = std::stof(args[2]);

  Model model = generator::Icosphere(radius, subdivisions);
  generator::Export(model, args[3]);
}

void handleBezierSurface(const std::vector<std::string>& args) {
  std::cout << "Generating model from patch file " << args[1]
            << ", tesselation: " << args[2] << "| Output: " << args[3]
            << std::endl;

  std::string patchFile = args[1];
  int tessellation = std::stoi(args[2]);

  Model model = generator::BezierSurface(patchFile, tessellation);
  generator::Export(model, args[3]);
}

int main(int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Error: No command provided.\n";
    printUsage();
    return 1;
  }

  std::unordered_map<
      std::string,
      std::pair<int, std::function<void(const std::vector<std::string>&)>>>
      commandMap = {{"sphere", {5, handleSphere}},
                    {"box", {4, handleBox}},
                    {"cone", {6, handleCone}},
                    {"plane", {4, handlePlane}},
                    {"cylinder", {6, handleCylinder}},
                    {"torus", {6, handleTorus}},
                    {"icosphere", {4, handleIcosphere}},
                    {"patch", {4, handleBezierSurface}}};

  std::vector<std::string> args(argv + 1, argv + argc);
  std::string command = args[0];

  auto it = commandMap.find(command);
  if (it != commandMap.end()) {
    int expectedArgs = it->second.first;
    if (args.size() != expectedArgs) {
      std::cerr << "Error: Incorrect number of arguments for '" << command
                << "' (expected " << expectedArgs - 1 << " after command).\n";
      printUsage();
      return 1;
    }
    it->second.second(args);
  } else {
    std::cerr << "Error: Unknown command '" << command << "'\n";
    printUsage();
    return 1;
  }

  return 0;
}
