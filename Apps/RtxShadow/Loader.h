#pragma once

#include "Scene/Mesh.h"
#include "Flythrough.h"

struct ModelData {
  Nei::Ptr<Nei::Mesh> mesh;
  std::vector < Nei::Ptr<Nei::Texture2D>> textures;
};

struct Loader {
  static ModelData load(Nei::DeviceContext* dc, fs::path const& path);
  static Flythrough loadFly(fs::path const& path);
};
